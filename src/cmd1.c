/* File: cmd1.c */

/* Purpose: Movement commands (part 1) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "script.h"

#define MAX_VAMPIRIC_DRAIN 100

/*
 * Calculate the deadliness based on the attack power.
 * This function takes into account bounds checking,
 * and then just looks at the deadliness_conversion
 * table in tables.c
 *
 * This number is 'inflated' by 100 from the eventual
 * multiplier to the damage dealt.
 */
int deadliness_calc(int attack_power)
{
	/* Calculate effect of deadliness - linearly */
	int result = (attack_power * 5) + 100;

	/* Really powerful minus yields zero damage */
	if (result < 0) result = 0;

	return (result);
}

/* Helper function to calculate the average damage a weapon will do */
long avg_dam(int attack_power, int dice_num, int dice_sides)
{
	/* Calculate damage per dice x 100 */
	long temp = dice_sides * deadliness_calc(attack_power);

	/* Add one to take into account dice formula, and return avg*200 */
	return (dice_num * (temp + 100));
}

/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
static bool test_hit_combat(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant hit.  Chance to miss removed in Oangband because
	 * of the way monster ACs now work (fewer truly easy targets).
	 */
	if (k < 5) return (TRUE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = chance / 2;

	/* Power competes against armor. */
	if ((chance > 0) && (randint0(chance) >= ac)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


/* At the moment this function is exactly the same as the melee function */
bool test_hit_fire(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(102);

	/* Hack -- Instant hit.  Chance to miss removed in Oangband because
	 * of the way monster ACs now work (fewer truly easy targets).
	 */
	if (k < 5) return (TRUE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = chance / 2;

	/* Power competes against armor. */
	if ((chance > 0) && (randint0(chance) >= ac)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


/*
 * Calculation of critical hits by the player in hand-to-hand combat. -LM-
 */
static int critical_melee(int chance, int sleeping_bonus, cptr m_name,
                          object_type *o_ptr)
{
	int power = (chance + sleeping_bonus);
	int bonus = 0;
	int psi_hit = FALSE;

	if ((FLAG(p_ptr, TR_PSI_CRIT)) && (p_ptr->csp >= PSI_COST) && 
			(randint(100) < 80))
	{
		psi_hit = TRUE;
	}

	if (FLAG(p_ptr, TR_STRANGE_LUCK))
		power = power * 3 / 2;

	/* Test for critical hit. */
	if (randint1(power + 240) <= power || (psi_hit && randint(100) < 20))
	{
		/*
		 * Encourage the player to make sneak attacks on
		 * sleeping monsters. -LM-
		 */
		if ((sleeping_bonus) && (p_ptr->rp.pclass == CLASS_ROGUE))
			msgf("You ruthlessly sneak attack!");

		/* Determine level of critical hit. */
		if (randint0(90) == 0)      bonus = 800;
		if (randint0(40) == 0)      bonus = 500;
		else if (randint0(12) == 0) bonus = 300;
		else if (randint0(3) == 0)  bonus = 200;
		else                        bonus = 100;


		/* Criticals with PSI_HIT weapons do about 47% more damage */
		if (psi_hit)
		{
			int psi_bonus;
			if (one_in_(12) && p_ptr->csp >= PSI_COST * 3)
				psi_bonus = 3;
			else if (one_in_(3) && p_ptr->csp >= PSI_COST * 2)
				psi_bonus = 2;
			else
				psi_bonus = 1;

			bonus *= psi_bonus;
			
			p_ptr->csp -= PSI_COST * psi_bonus;
			p_ptr->redraw |= (PR_MANA);
			p_ptr->window |= (PW_PLAYER);
			p_ptr->window |= (PW_SPELL);
		}


		if ((bonus <= 100) ||
			((o_ptr->tval == TV_HAFTED) && (o_ptr->sval == SV_WHIP)))
		{
			msgf(MSGT_HIT, "You strike %s.", m_name);
		}
		else if (bonus <= 200)
		{
			if ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM))
				msgf(MSGT_HIT, "You hack at %s.", m_name);
			else
				msgf(MSGT_HIT, "You bash %s.", m_name);
		}
		else if (bonus <= 300)
		{
			if ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM))
				msgf(MSGT_HIT, "You slash %s.", m_name);
			else
				msgf(MSGT_HIT, "You pound %s.", m_name);
		}
		else if (bonus <= 500)
		{
			if ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM))
				msgf(MSGT_HIT, "You gouge %s!", m_name);
			else
				msgf(MSGT_HIT, "You bludgeon %s!", m_name);
		}
		else
		{
			msgf(MSGT_HIT, "You *smite* %s!", m_name);
		}
	}

	/*
	 * If the blow is not a critical hit, display the default attack
	 * message and apply the standard multiplier.
	 */
	else
	{
		msgf(MSGT_HIT, "You hit %s.", m_name);
	}

	return (bonus);
}


/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 * This is used by the old Monk attack routine
 * mutuations, + a few of the ego weapons.
 */
static s16b critical_norm(int weight, int plus, int dam)
{
	int power, k;

	/* Extract "blow" power */
	power = (weight + ((p_ptr->to_h + plus) * 5) + (p_ptr->lev * 3));

	if (FLAG(p_ptr, TR_STRANGE_LUCK))
		power = power * 3 / 2;

	/* Chance */
	if (randint1(5000) <= power)
	{
		k = weight + randint1(650);

		if (k < 400)
		{
			msgf("It was a good hit!");
			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
			msgf("It was a great hit!");
			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
			msgf("It was a superb hit!");
			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
			msgf("It was a *GREAT* hit!");
			dam = 3 * dam + 20;
		}
		else
		{
			msgf("It was a *SUPERB* hit!");
			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}


/*
 * Extract the deadliness bonus from slays and brands.
 *
 * Note that most brands and slays are +200%, except Slay Animal (+100%),
 * Slay Evil (+100%), and Kill dragon (+400%).
 *
 * Currently brands add +200%, but it might be more interesting to make
 * them +100% that is cumulative with other slays and brands.
 */
int tot_dam_aux(const object_type *o_ptr, const monster_type *m_ptr)
{
	/*
	 * mult is based at 100 = +0 deadliness.
	 */
	int mult = 100;


	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Extract the flags */
	u32b f1 = o_ptr->flags[0];

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			/* Slay Animal */
			if ((f1 & TR0_SLAY_ANIMAL) && (FLAG(r_ptr, RF_ANIMAL)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_ANIMAL;
				}

				if (mult < 200) mult = 200;
			}

			/* Slay Evil */
			if ((f1 & TR0_SLAY_EVIL) && (FLAG(r_ptr, RF_EVIL)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_EVIL;
				}

				if (mult < 200) mult = 200;
			}

			/* Slay Undead */
			if ((f1 & TR0_SLAY_UNDEAD) && (FLAG(r_ptr, RF_UNDEAD)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_UNDEAD;
				}

				if (mult < 300) mult = 300;
			}

			/* Slay Demon */
			if ((f1 & TR0_SLAY_DEMON) && (FLAG(r_ptr, RF_DEMON)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_DEMON;
				}

				if (mult < 300) mult = 300;
			}

			/* Slay Orc */
			if ((f1 & TR0_SLAY_ORC) && (FLAG(r_ptr, RF_ORC)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_ORC;
				}

				if (mult < 300) mult = 300;
			}

			/* Slay Troll */
			if ((f1 & TR0_SLAY_TROLL) && (FLAG(r_ptr, RF_TROLL)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_TROLL;
				}

				if (mult < 300) mult = 300;
			}

			/* Slay Giant */
			if ((f1 & TR0_SLAY_GIANT) && (FLAG(r_ptr, RF_GIANT)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_GIANT;
				}

				if (mult < 300) mult = 300;
			}

			/* Slay Dragon */
			if ((f1 & TR0_SLAY_DRAGON) && (FLAG(r_ptr, RF_DRAGON)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_DRAGON;
				}

				if (mult < 300) mult = 300;
			}

			/* Execute Dragon */
			if ((f1 & TR0_KILL_DRAGON) && (FLAG(r_ptr, RF_DRAGON)))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags[2] |= RF2_DRAGON;
				}

				if (mult < 500) mult = 500;
			}

			/* Brand (Acid) */
			if (f1 & TR0_BRAND_ACID)
			{
				/* Notice immunity */
				if (FLAG(r_ptr, RF_IM_ACID))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_IM_ACID;
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 300) mult = 300;
				}
			}

			/* Brand (Elec) */
			if (f1 & TR0_BRAND_ELEC)
			{
				/* Notice immunity */
				if (FLAG(r_ptr, RF_IM_ELEC))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_IM_ELEC;
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 300) mult = 300;
				}
			}

			/* Brand (Fire) */
			if (f1 & TR0_BRAND_FIRE)
			{
				/* Notice immunity */
				if (FLAG(r_ptr, RF_IM_FIRE))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_IM_FIRE;
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 300) mult = 300;
				}
			}

			/* Brand (Cold) */
			if (f1 & TR0_BRAND_COLD)
			{
				/* Notice immunity */
				if (FLAG(r_ptr, RF_IM_COLD))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_IM_COLD;
					}
				}
				/* Otherwise, take the damage */
				else
				{
					if (mult < 300) mult = 300;
				}
			}

			/* Brand (Poison) */
			if (f1 & TR0_BRAND_POIS)
			{
				/* Notice immunity */
				if (FLAG(r_ptr, RF_IM_POIS))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_IM_POIS;
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 300) mult = 300;
				}
			}
			break;
		}
	}


	/* Return the total damage */
	return (mult);
}


/*
 * Search for hidden things
 */
void search(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int y, x, chance;
	int old_count;

	/* 
	 * Temp variables to store x and y because the
	 * count_traps() function stomps on its inputs.
	 */
	int tx, ty;

	cave_type *c_ptr;
	object_type *o_ptr;

	/* Start with base search ability */
	chance = p_ptr->skills[SKILL_SNS];

	/* Penalize various conditions */
	if (p_ptr->tim.blind || no_lite()) chance = chance / 10;
	if (p_ptr->tim.confused || p_ptr->tim.image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (py - 1); y <= (py + 1); y++)
	{
		for (x = (px - 1); x <= (px + 1); x++)
		{
			/* Sometimes, notice things */
			if (randint0(100) < chance)
			{
				/* do not search outside the wilderness */
				if (!in_bounds2(x, y)) continue;

				/* Access the grid */
				c_ptr = area(x, y);

				/* Save x and y into temp variables */
				tx = x;
				ty = y;

				/* Count number of visible traps next to player */
				old_count = count_traps(&tx, &ty, TRUE);

				/* Look for invisible traps */
				if (field_detect_type(c_ptr, FTYPE_TRAP))
				{
					/* Save x and y into temp variables */
					tx = x;
					ty = y;

					/* See if the number of known traps has changed */
					if (old_count != count_traps(&tx, &ty, TRUE))
					{
						/* Message */
						msgf("You have found a trap.");

						/* Disturb */
						disturb(FALSE);
					}
				}

				/* Secret door */
				if (c_ptr->feat == FEAT_SECRET)
				{
					/* Message */
					msgf("You have found a secret door.");

					/* Pick a door */
					create_closed_door(x, y);

					/* Disturb */
					disturb(FALSE);
				}

				/* Scan all objects in the grid */
				OBJ_ITT_START (c_ptr->o_idx, o_ptr)
				{
					/* Skip non-chests */
					if (o_ptr->tval != TV_CHEST) continue;

					/* Skip non-trapped chests */
					if (!chest_traps[o_ptr->pval]) continue;

					/* Identify once */
					if (!object_known_p(o_ptr))
					{
						/* Message */
						msgf("You have discovered a trap on the chest!");

						/* Know the trap */
						object_known(o_ptr);

						/* Notice it */
						disturb(FALSE);
					}
				}
				OBJ_ITT_END;
			}
		}
	}
}


/*
 * Determine if the object can be picked up, and has "=g" in its inscription.
 */
bool auto_pickup_okay(const object_type *o_ptr)
{
	cptr s;

	/* It can't be carried */
	if (!inven_carry_okay(o_ptr)) return (FALSE);

	/* No inscription */
	if (!o_ptr->inscription) return (FALSE);

	/* Find a '=' */
	s = strchr(quark_str(o_ptr->inscription), '=');

	/* Process inscription */
	while (s)
	{
		/* Auto-pickup on "=g" */
		if (s[1] == 'g') return (TRUE);

		/* Find another '=' */
		s = strchr(s + 1, '=');
	}

	/* Don't auto pickup */
	return (FALSE);
}


/*
 * Helper routine for py_pickup().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
void py_pickup_aux(object_type *o_ptr)
{
	object_type *j_ptr;

	int slot;

	/* Duplicate the object */
	j_ptr = object_dup(o_ptr);

	/* Delete the old object */
	delete_dungeon_object(o_ptr);

	/* Carry the object */
	j_ptr = inven_carry(j_ptr);

	/*
	 * Note: we have just made an empty slot in o_list
	 * so j_ptr should never be NULL after inven_carry()
	 */

	/* Get slot number */
	slot = get_item_position(p_ptr->inventory, j_ptr);

	/* Message */
	msgf("You have %v (%c).", OBJECT_FMT(j_ptr, TRUE, 3), I2A(slot));
}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_player()" for handling of other things.
 *
 * If the easy floor option is true: 
 *		Make the player carry everything in a grid
 *
 * 		If "pickup" is FALSE then only gold will be picked up
 */
void carry(int pickup)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	char o_name[256];
	object_type *o_ptr, *fo_ptr = NULL;

	int floor_num = 0;

	int can_pickup = 0;

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= PU_MONSTERS;

	/* Redraw map */
	p_ptr->redraw |= PR_MAP;

	/* Window stuff */
	p_ptr->window |= PW_OVERHEAD;

	/* Handle stuff */
	handle_stuff();

	/* Scan the pile of objects */
	OBJ_ITT_START (area(px, py)->o_idx, o_ptr)
	{
		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3, 256);

		/* Hack -- disturb */
		disturb(FALSE);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Message */
			msgf("You have found %ld gold pieces worth of %s.",
					   (long)o_ptr->pval, o_name);

			sound(SOUND_SELL);

			/* Collect the gold */
			p_ptr->au += o_ptr->pval;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Delete the gold */
			delete_dungeon_object(o_ptr);

			/* Check the next object */
			continue;
		}

		/* Test for auto-pickup */
		if (auto_pickup_okay(o_ptr))
		{
			/* Hack - Pick up the object */
			py_pickup_aux(o_ptr);

			/* Check the next object */
			continue;
		}

		/* The old pick-up routine is here */
		if (!easy_floor)
		{
			/* Describe the object */
			if (!pickup)
			{
				msgf("You find %s.", o_name);
			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(o_ptr))
			{
				msgf("You have no room for %s.", o_name);
			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				/* Hack -- query every item */
				if (carry_query_flag)
				{
					int i;

					/* Paranoia XXX XXX XXX */
					message_flush();

					/* Prompt for it */
					prtf(0, 0, "Pick up %s? [y/n/k] ", o_name);

					/* Get an acceptable answer */
					while (TRUE)
					{
						i = inkey();
						if (quick_messages) break;
						if (i == ESCAPE) break;
						if (strchr("YyNnKk", i)) break;
						bell("Illegal pick-up command!");
					}

					/* Erase the prompt */
					clear_msg();

					if ((i == 'Y') || (i == 'y'))
					{
						/* Hack - Pick up the object */
						py_pickup_aux(o_ptr);
					}

					if ((i == 'K') || (i == 'k'))
					{
						/* Physically try to destroy the item */
						if (destroy_item_aux(o_ptr, o_ptr->number))
						{
							delete_dungeon_object(o_ptr);
						}
					}
				}

				/* Attempt to pick up an object. */
				else
				{
					/* Hack - Pick up the object */
					py_pickup_aux(o_ptr);
				}
			}

			/* Get the next object */
			continue;
		}


		/* Count non-gold objects that can be picked up. */
		if (inven_carry_okay(o_ptr))
		{
			can_pickup++;
		}

		/* Count non-gold objects */
		if (floor_num != 22) floor_num++;

		/* Hack - Remember this object */
		fo_ptr = o_ptr;
	}
	OBJ_ITT_END;

	/* There are no non-gold objects left */
	if (!floor_num) return;

	/* Mention the number of objects */
	if (!pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Message */
			msgf("You find %v.", OBJECT_FMT(fo_ptr, TRUE, 3));
		}

		/* Multiple objects */
		else
		{
			/* Message */
			msgf("You find a pile of %d items.", floor_num);
		}

		/* Done */
		return;
	}

	/* The player has no room for anything on the floor. */
	if (!can_pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Message */
			msgf("You have no room for %v.", OBJECT_FMT(fo_ptr, TRUE, 3));
		}

		/* Multiple objects */
		else
		{
			/* Message */
			msgf("You have no room for any of the objects on the floor.");
		}

		/* Done */
		return;
	}

	/* One object */
	if (floor_num == 1)
	{
		/* Hack -- query every object */
		if (carry_query_flag)
		{
			int i;

			/* Paranoia XXX XXX XXX */
			message_flush();

			/* Prompt for it */
			prtf(0, 0,"Pick up %v? [y/n/k] ", OBJECT_FMT(fo_ptr, TRUE, 3));

			/* Get an acceptable answer */
			while (TRUE)
			{
				i = inkey();
				if (quick_messages) break;
				if (i == ESCAPE) break;
				if (strchr("YyNnKk", i)) break;
				bell("Illegal pick-up command!");
			}

			/* Erase the prompt */
			clear_msg();

			if ((i == 'Y') || (i == 'y'))
			{
				/* Pick up the object */
				py_pickup_aux(fo_ptr);
			}

			if ((i == 'K') || (i == 'k'))
			{
				/* Physically try to destroy the item */
				if (destroy_item_aux(fo_ptr, fo_ptr->number))
				{
					delete_dungeon_object(fo_ptr);
				}
			}

			/* Done */
			return;
		}

		/* Pick up the object */
		py_pickup_aux(fo_ptr);

		/* Done */
		return;
	}

	/* Restrict the choices */
	item_tester_hook = inven_carry_okay;

	/* Get an object */
	o_ptr = get_item("Get which item? ", "You see nothing there.", (USE_FLOOR));

	/* Not a valid item */
	if (!o_ptr) return;

	/* Pick up the object */
	py_pickup_aux(o_ptr);
}



static void touch_zap_player(const monster_type *m_ptr)
{
	int aura_damage;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (FLAG(r_ptr, RF_AURA_FIRE))
	{
		if (!(FLAG(p_ptr, TR_IM_FIRE)))
		{
			char aura_dam[80];

			aura_damage =
				damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x88, 80);

			msgf("You are suddenly very hot!");

			take_hit(resist(aura_damage, res_fire_lvl), aura_dam);
			r_ptr->r_flags[1] |= RF1_AURA_FIRE;
			handle_stuff();
		}
	}

	if (FLAG(r_ptr, RF_AURA_COLD))
	{
		if (!(FLAG(p_ptr, TR_IM_COLD)))
		{
			char aura_dam[80];

			aura_damage =
				damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x88, 80);

			msgf("You are suddenly very cold!");

			take_hit(resist(aura_damage, res_cold_lvl), aura_dam);
			r_ptr->r_flags[2] |= RF2_AURA_COLD;
			handle_stuff();
		}
	}

	if (FLAG(r_ptr, RF_AURA_ELEC))
	{
		if (!(FLAG(p_ptr, TR_IM_ELEC)))
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x88, 80);

			msgf("You get zapped!");
			take_hit(resist(aura_damage, res_elec_lvl), aura_dam);
			r_ptr->r_flags[1] |= RF1_AURA_ELEC;
			handle_stuff();
		}
	}
}


static void natural_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	int k, bonus, chance;
	int n_weight;
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	char m_name[80];
	int dss, ddd;
	cptr atk_desc;

	switch (attack)
	{
		case MUT2_SCOR_TAIL:
		{
			dss = 3;
			ddd = 7;
			n_weight = 5;
			atk_desc = "tail";
			break;
		}
		case MUT2_HORNS:
		{
			dss = 2;
			ddd = 6;
			n_weight = 15;
			atk_desc = "horns";
			break;
		}
		case MUT2_BEAK:
		{
			dss = 2;
			ddd = 4;
			n_weight = 5;
			atk_desc = "beak";
			break;
		}
		case MUT2_TRUNK:
		{
			dss = 1;
			ddd = 4;
			n_weight = 35;
			atk_desc = "trunk";
			break;
		}
		case MUT2_TENTACLES:
		{
			dss = 2;
			ddd = 5;
			n_weight = 5;
			atk_desc = "tentacles";
			break;
		}
		default:
		{
			dss = ddd = n_weight = 1;
			atk_desc = "undefined body part";
		}
	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0, 80);


	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h;
	chance = (p_ptr->skills[SKILL_THN] + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	if ((!(FLAG(r_ptr, RF_QUANTUM)) || one_in_(2)) &&
		test_hit_combat(chance, r_ptr->ac, m_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

		msgf(MSGT_HIT, "You hit %s with your %s.", m_name, atk_desc);
		msg_effect(MSG_HIT, m_ptr->r_idx);

		k = damroll(ddd, dss);
		k = critical_norm(n_weight, p_ptr->to_h, k);

		/* Apply the player damage bonuses */
		k += p_ptr->to_d;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(m_ptr, k, 0);

		/* Complex message */
		if (p_ptr->state.wizard)
		{
			msgf("You do %d (out of %d) damage.", k, m_ptr->hp);
		}

		/* Anger the monster */
		if (k > 0) anger_monster(m_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case MUT2_SCOR_TAIL:
			{
				project(0, 0, m_ptr->fx, m_ptr->fy, k, GF_POIS, PROJECT_KILL);
				*mdeath = (m_ptr->r_idx == 0);
				break;
			}
			case MUT2_HORNS:
			{
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			}
			case MUT2_BEAK:
			{
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			}
			case MUT2_TRUNK:
			{
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			}
			case MUT2_TENTACLES:
			{
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			}
			default:
			{
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
			}
		}

		make_noise(4);

		touch_zap_player(m_ptr);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
		msgf(MSGT_MISS, "You miss %s.", m_name);
		msg_effect(MSG_MISS, m_ptr->r_idx);
	}
}


static bool monster_bash(int *blows, int sleeping_bonus, const cave_type *c_ptr,
                         bool *fear, cptr m_name)
{
	int bash_chance, bash_quality, bash_dam;

	monster_type *m_ptr = &m_list[c_ptr->m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	object_type *o_ptr;

	/* No shield on arm, no bash. */
	if (!p_ptr->equipment[EQUIP_ARM].k_idx)
	{
		bash_chance = 0;
	}

	/*
	 * Players do not bash if they could otherwise take advantage of special
	 * bonuses against sleeping monsters, or if the monster is low-level.
	 */
	else if ((sleeping_bonus) || (r_ptr->level < p_ptr->lev / 2))
	{
		bash_chance = 0;
	}

	/* Bashing chance depends on melee Skill, Dex, and a class level bonus. */
	else
		bash_chance = p_ptr->skills[SKILL_THN] +
			(adj_dex_th[p_ptr->stat[A_DEX].ind]) - 128 +
			(((p_ptr->rp.pclass == CLASS_WARRIOR) ||
			  (p_ptr->rp.pclass == CLASS_PALADIN) ||
			  (p_ptr->rp.pclass == CLASS_WARRIOR_MAGE) ||
			  (p_ptr->rp.pclass == CLASS_CHAOS_WARRIOR)) ? p_ptr->lev : 0);

	/* Players bash more often when they see a real need. */
	if (bash_chance)
	{
		o_ptr = &p_ptr->equipment[EQUIP_WIELD];

		if ((!o_ptr->k_idx) && (p_ptr->rp.pclass != CLASS_MONK))
			bash_chance *= 3;
		else if ((o_ptr->dd * o_ptr->ds * (*blows)) <
				 (p_ptr->equipment[EQUIP_ARM].dd *
				  p_ptr->equipment[EQUIP_ARM].ds * 3))
			bash_chance *= 2;
	}

	/* Try to get in a shield bash. */
	if (bash_chance > randint0(240 + r_ptr->level * 9))
	{
		o_ptr = &p_ptr->equipment[EQUIP_ARM];

		msgf("You get in a shield bash!");

		/* Calculate attack quality, a mix of momentum and accuracy. */
		bash_quality = p_ptr->skills[SKILL_THN] + (p_ptr->rp.wt / 8) +
			(p_ptr->total_weight / 80) + (o_ptr->weight / 3);

		/* Calculate damage.  Big shields are deadly. */
		bash_dam = damroll(o_ptr->dd, o_ptr->ds);

		/* Multiply by quality and experience factors */
		bash_dam *= bash_quality / 20 + p_ptr->lev / 7;

		/* Strength bonus. */
		bash_dam += (adj_str_td[p_ptr->stat[A_STR].ind] - 128);

		/* Paranoia. */
		if (bash_dam > 125) bash_dam = 125;

		/* Encourage the player to keep wearing that heavy shield. */
		if (randint1(bash_dam) > 30 + randint1(bash_dam / 2))
			msgf("WHAMM!");

		/* Complex message */
		if (p_ptr->state.wizard)
		{
			msgf("You do %d (out of %d) damage.", bash_dam, m_ptr->hp);
		}

		/* Damage, check for fear and death. */
		if (mon_take_hit(c_ptr->m_idx, bash_dam, fear, NULL))
		{
			/* Fight's over. */
			return (TRUE);
		}

		/* Stunning. */
		if (bash_quality + p_ptr->lev > randint1(200 + r_ptr->level * 8))
		{
			msgf("%^s is stunned.", m_name);

			m_ptr->stunned += randint0(p_ptr->lev / 5) + 4;
			if (m_ptr->stunned > 24) m_ptr->stunned = 24;
		}

		/* Confusion. */
		if (bash_quality + p_ptr->lev > randint1(300 + r_ptr->level * 6) &&
			!(FLAG(r_ptr, RF_NO_CONF)))
		{
			msgf("%^s appears confused.", m_name);

			m_ptr->confused += randint0(p_ptr->lev / 5) + 4;
		}

		/* The player will sometimes stumble. */
		if ((30 + adj_dex_th[p_ptr->stat[A_DEX].ind] - 128) < randint1(60))
			*blows -= randint1(*blows);
	}

	make_noise(4);

	/* Monster is not dead */
	return (FALSE);
}

/*
 * The monk special attacks and effects.
 */

static void monk_attack(monster_type *m_ptr, long *k, cptr m_name)
{
	int special_effect = 0, stun_effect = 0, times = 0;
	const martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
	int resist_stun = 0;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (FLAG(r_ptr, RF_UNIQUE)) resist_stun += 88;
	if (FLAG(r_ptr, RF_NO_CONF)) resist_stun += 44;
	if (FLAG(r_ptr, RF_NO_SLEEP)) resist_stun += 44;
	if ((FLAG(r_ptr, RF_UNDEAD)) || (FLAG(r_ptr, RF_NONLIVING)))
		resist_stun += 88;

	/* Attempt 'times' */
	for (times = 0; times < (p_ptr->lev < 7 ? 1 : p_ptr->lev / 7); times++)
	{
		do
		{
			ma_ptr = &ma_blows[randint0(MAX_MA)];
		}
		while ((ma_ptr->min_level > p_ptr->lev) ||
			   (randint1(p_ptr->lev) < ma_ptr->chance));

		/* keep the highest level attack available we found */
		if ((ma_ptr->min_level > old_ptr->min_level) &&
			!p_ptr->tim.stun && !p_ptr->tim.confused)
		{
			old_ptr = ma_ptr;

			if (p_ptr->state.wizard && cheat_xtra)
			{
				msgf("Attack re-selected.");
			}
		}
		else
		{
			ma_ptr = old_ptr;
		}
	}

	*k = damroll(ma_ptr->dd, ma_ptr->ds);

	if (ma_ptr->effect == MA_KNEE)
	{
		if (FLAG(r_ptr, RF_MALE))
		{
			msgf(MSGT_HIT, "You hit %s in the groin with your knee!", m_name);
			msg_effect(MSG_HIT, m_ptr->r_idx);
			sound(SOUND_PAIN);
			special_effect = MA_KNEE;
		}
		else
		{
			msgf(MSGT_HIT, ma_ptr->desc, m_name);
			msg_effect(MSG_HIT, m_ptr->r_idx);
		}
	}

	else if (ma_ptr->effect == MA_SLOW)
	{
		if (!((FLAG(r_ptr, RF_NEVER_MOVE)) ||
			  strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
		{
			msgf(MSGT_HIT, "You kick %s in the ankle.",
						   m_name);
			msg_effect(MSG_HIT, m_ptr->r_idx);
			special_effect = MA_SLOW;
		}
		else
		{
			msgf(MSGT_HIT, ma_ptr->desc, m_name);
			msg_effect(MSG_HIT, m_ptr->r_idx);
		}
	}
	else
	{
		if (ma_ptr->effect)
		{
			stun_effect = rand_range(ma_ptr->effect / 2, ma_ptr->effect);
		}

		msgf(MSGT_HIT, ma_ptr->desc, m_name);
		msg_effect(MSG_HIT, m_ptr->r_idx);
	}

	*k = critical_norm(p_ptr->lev * randint1(10), ma_ptr->min_level, *k);

	if ((special_effect == MA_KNEE) && (*k  < m_ptr->hp))
	{
		msgf("%^s moans in agony!", m_name);
		stun_effect = rand_range(8, 20);
		resist_stun /= 3;
	}

	else if ((special_effect == MA_SLOW) && (*k < m_ptr->hp))
	{
		if (!(FLAG(r_ptr, RF_UNIQUE)) &&
			(randint1(p_ptr->lev) > r_ptr->level) && m_ptr->mspeed > 60)
		{
			msgf("%^s starts limping slower.", m_name);
			m_ptr->mspeed -= 10;
		}
	}

	if (stun_effect && (*k < m_ptr->hp))
	{
		if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
		{
			if (m_ptr->stunned)
				msgf("%^s is more stunned.", m_name);
			else
				msgf("%^s is stunned.", m_name);

			m_ptr->stunned += stun_effect;
		}
	}
}


/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
void py_attack(int x, int y)
{
	/* Sides dice, also total damage. */
	long k;

	/* The whole and fractional damage dice and their resulting damage. */
	int slay;

	/* blow count */
	int num = 0;

	/* Bonus to attack if monster is sleeping, for certain classes. */
	int sleeping_bonus = 0;

	/* Bonus to effective monster ac if it can take cover in terrain. */
	int terrain_bonus = 0;

	int bonus, chance, total_deadliness;

	int blows;

	cave_type *c_ptr = area(x, y);

	monster_type *m_ptr = &m_list[c_ptr->m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	object_type *o_ptr;

	char m_name[80];


	bool fear = FALSE;
	bool mdeath = FALSE;

	bool do_quake = FALSE;
	bool drain_msg = TRUE;
	int drain_result = 0, drain_heal = 0;
	int drain_total = 0;
	s16b ghoul_paral = -1;
	bool ghoul_hack = FALSE;
	bool no_extra = FALSE;

	/* Access the weapon */
	o_ptr = &p_ptr->equipment[EQUIP_WIELD];



	/* Disturb the player */
	disturb(FALSE);

	/* Initial blows available. */
	blows = p_ptr->num_blow;

	/* Prepare for ghoul paralysis? */
	if (!(o_ptr->k_idx) && (FLAG(p_ptr, TR_GHOUL_TOUCH)))
	{
		ghoul_paral = 0;

		/* Prevent bogus falls asleep messages */
		if (!(m_ptr->csleep)) ghoul_hack = TRUE;
	}

	/* It is not honorable etc to attack helpless victims -- in most cases */
	if ((m_ptr->csleep) && (ghoul_paral > -1))
	{
		chg_virtue(V_COMPASSION, -1);
		if (!(p_ptr->rp.pclass == CLASS_ROGUE)) chg_virtue(V_HONOUR, -1);
	}

	if (p_ptr->rp.pclass == CLASS_ROGUE)
	{
		if (m_ptr->csleep && m_ptr->ml)
		{
			/* Can't backstab creatures that we can't see, right? */
			sleeping_bonus = 10 + 2 * p_ptr->lev / 5;
		}
		else if (m_ptr->monfear && m_ptr->ml)
		{
			sleeping_bonus = 5 + p_ptr->lev / 5;
		}
	}

	/* Disturb the monster */
	m_ptr->csleep = 0;

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0, 80);

	/* Auto-Recall if possible and visible */
	if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

	/* Track a new monster */
	if (m_ptr->ml) health_track(c_ptr->m_idx);

	/* Look to see if we've spotted a mimic */
	if (m_ptr->smart & SM_MIMIC)
	{
		/* We've spotted it */
		msgf("You've found %v!", MONSTER_FMT(m_ptr, 0x88));
		
		/* Toggle flag */
		m_ptr->smart &= ~(SM_MIMIC);

		/* It is in the monster list now if visible */
		if (m_ptr->ml) update_mon_vis(m_ptr->r_idx, 1);
	}

	/* Stop if friendly and visible */
	if (!is_hostile(m_ptr) && !p_ptr->tim.stun && !p_ptr->tim.confused
		&& !p_ptr->tim.image && !((p_ptr->muta2 & MUT2_BERS_RAGE) && p_ptr->tim.shero)
		&& m_ptr->ml)
	{
		if (!o_ptr->xtra_name)
		{
			msgf("You stop to avoid hitting %s.", m_name);
			return;
		}

		/* Mega-hack */
		if (!(streq(quark_str(o_ptr->xtra_name), "'Stormbringer'")))
		{
			msgf("You stop to avoid hitting %s.", m_name);
			return;
		}

		msgf("Your black blade greedily attacks %s!", m_name);

		chg_virtue(V_INDIVIDUALISM, 1);
		chg_virtue(V_HONOUR, -1);
		chg_virtue(V_JUSTICE, -1);
		chg_virtue(V_COMPASSION, -1);
	}

	/* Handle player fear */
	if (p_ptr->tim.afraid)
	{
		/* Message */
		if (m_ptr->ml)
			msgf("You are too afraid to attack %s!", m_name);
		else
			msgf("There is something scary in your way!");

		/* Done */
		return;
	}

	/* Using a weapon can cause it to become cursed */
	if ((FLAG(o_ptr, TR_AUTO_CURSE)) && 
			!(FLAG(o_ptr, TR_CURSED)) && (randint(100) < 10))
	{
		msgf("Your weapon glows black.");
		SET_FLAG(o_ptr, TR_CURSED);
		o_ptr->feeling = FEEL_NONE;
	}

	/* Monsters in rubble can take advantage of cover. -LM- */
	if (c_ptr->feat == FEAT_RUBBLE)
	{
		terrain_bonus = r_ptr->ac / 7 + 5;
	}
	/*
	 * Monsters in trees can take advantage of cover,
	 * except from rangers.
	 */
	else if ((c_ptr->feat == FEAT_TREES) && (p_ptr->rp.pclass != CLASS_RANGER))
	{
		terrain_bonus = r_ptr->ac / 7 + 5;
	}
	/* Monsters in water are vulnerable. -LM- */
	else if (c_ptr->feat == FEAT_DEEP_WATER)
	{
		terrain_bonus -= r_ptr->ac / 5;
	}

	/* Attempt to shield bash the monster */
	if (monster_bash(&blows, sleeping_bonus, c_ptr, &fear, m_name)) return;

	/* Initialize. */
	total_deadliness = p_ptr->to_d + o_ptr->to_d;

	/* Calculate the "attack quality".  As BTH_PLUS_ADJ has been reduced
	 * to 1, base skill and modifiers to skill are given equal weight. -LM-
	 */
	bonus = p_ptr->to_h + o_ptr->to_h;
	chance = (p_ptr->skills[SKILL_THN] + (bonus * BTH_PLUS_ADJ));

	apply_object_trigger(TRIGGER_ATTACK, o_ptr, "p:iiii", 
		LUA_MONSTER_NAMED(m_ptr, "monster"), LUA_RETURN(chance),
		LUA_RETURN(terrain_bonus), LUA_RETURN(total_deadliness),
		LUA_RETURN(sleeping_bonus));

	/* Attack once for each legal blow */
	while (num++ < blows)
	{
		/* Test for hit */
		if (test_hit_combat(chance + sleeping_bonus,
							r_ptr->ac + terrain_bonus, m_ptr->ml))
		{
			int drain_power = 0;
			bool do_poly = FALSE;
			bool do_tele = FALSE;
			bool do_conf = FALSE;

			/* Sound */
			sound(SOUND_HIT);

			/* Hack -- bare hands do one damage */
			k = 1;

			/* Select a chaotic effect (50% chance) */
			if ((FLAG(o_ptr, TR_CHAOTIC)) && (one_in_(2)))
			{
				if (one_in_(10)) chg_virtue(V_CHANCE, 1);

				if (randint1(5) < 3)
				{
					/* Vampiric (20%) */
					if (monster_living(r_ptr))
						drain_power = 4;
					else
						drain_power = 0;
				}
				else if (one_in_(250))
				{
					/* Quake (0.12%) */
					do_quake = TRUE;
				}
				else if (!one_in_(10))
				{
					/* Confusion (26.892%) */
					do_conf = TRUE;
				}
				else if (one_in_(2))
				{
					/* Teleport away (1.494%) */
					do_tele = TRUE;
				}
				else
				{
					/* Polymorph (1.494%) */
					do_poly = TRUE;
				}
			}

			/* Vampiric drain */
			if (FLAG(o_ptr, TR_VAMPIRIC))
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					drain_power = 4;
				else
					drain_power = 0;
			}

			/* Save current hp */
			drain_result = m_ptr->hp;

			/* Ghoul paralysis */
			if ((ghoul_paral > -1) && !(FLAG(r_ptr, RF_NO_SLEEP)) &&
				(r_ptr->level < randint0(1 + ((p_ptr->lev) * 2))))
			{
				ghoul_paral += 25 + randint1(p_ptr->lev / 2);
			}

			/* Monk attack? */
			if ((p_ptr->rp.pclass == CLASS_MONK) && (!o_ptr->k_idx))
			{
				/* Make a special monk attack */
				monk_attack(m_ptr, &k, m_name);
			}

			/* Handle normal weapon */
			else if (o_ptr->k_idx)
			{
				int vorpal_chance = (o_ptr->flags[0] & TR0_VORPAL) ? 2 : 0;
				int multiplier = deadliness_calc(total_deadliness);

				/* base damage dice. */
				k = o_ptr->ds;

				/* Add deadliness bonus from slays/brands */
				slay = (tot_dam_aux(o_ptr, m_ptr) - 100);
				multiplier += slay;


				/* Add deadliness bonus from critical hits */
				multiplier += critical_melee(chance, sleeping_bonus, m_name, o_ptr);

				/*
				 * Convert total Deadliness into a percentage, and apply
				 * it as a bonus or penalty. (100x inflation)
				 */
				k *= multiplier;

				/*
				 * Get the whole number of dice sides by deflating,
				 * and then get total dice damage.
				 */
				k = damroll(o_ptr->dd, k / 100 +
							(randint0(100) < (k % 100) ? 1 : 0));

				/* Add in extra effect due to slays */
				k += slay / 20;

				/* Apply scripted effects */
				apply_object_trigger(TRIGGER_HIT, o_ptr, "p:iiiibbbb", 
					LUA_MONSTER_NAMED(m_ptr, "monster"), LUA_RETURN(k), 
					LUA_RETURN(ghoul_paral), LUA_RETURN(drain_power),
					LUA_RETURN(vorpal_chance), LUA_RETURN(do_quake),
					LUA_RETURN(do_conf), LUA_RETURN(do_tele),
					LUA_RETURN(do_poly));

				/* hack -- check for earthquake. */
				if ((FLAG(p_ptr, TR_IMPACT)) &&
					((k > 50) || one_in_(7)))
				{
					do_quake = TRUE;
				}

				/* 
				 * All of these artifact-specific effects
				 * should be pythonized.
				 */
				if (vorpal_chance && one_in_(3 * vorpal_chance))
				{
					/*
					 * The vorpal blade does average:
					 *  (e+2)/3 x normal damage.
					 * A normal weapon with the vorpal flag does average:
					 *   e-3/2 x normal damage.
					 * Note: this has changed from before - the vorpal blade
					 *  has been toned down because of the oangband based
					 *  combat.
					 */
					int mult = 2;

					int inc_chance = 2 * vorpal_chance;

					if (vorpal_chance < 2)
					{
						msgf("Your Vorpal Blade goes snicker-snack!");
					}
					else
					{
						msgf("Your weapon cuts deep into %s!", m_name);
					}

					/* Try to increase the damage */
					while (one_in_(inc_chance))
					{
						mult++;
						inc_chance++;
					}

					k *= mult;

					/* Ouch! */
					if (k > m_ptr->hp)
					{
						msgf("You cut %s in half!", m_name);
					}
					else
					{
						switch (mult)
						{
							case 2:
							{
								msgf("You gouge %s!", m_name);
								break;
							}
							case 3:
							{
								msgf("You maim %s!", m_name);
								break;
							}
							case 4:
							{
								msgf("You carve %s!", m_name);
								break;
							}
							case 5:
							{
								msgf("You cleave %s!", m_name);
								break;
							}
							case 6:
							{
								msgf("You smite %s!", m_name);
								break;
							}
							case 7:
							{
								msgf("You eviscerate %s!", m_name);
								break;
							}
							default:
							{
								msgf("You shred %s!", m_name);
							}
						}
					}
				}
			}

			/* Bare hands and not a monk */
			else
			{
				msgf("You %s %s.",
						   ((p_ptr->rp.prace == RACE_GHOUL) ? "claw" : "punch"),
						   m_name);
			}

			/* No negative damage */
			if (k < 0) k = 0;

			/* Modify the damage */
			k = mon_damage_mod(m_ptr, k, 0);

			/* Complex message */
			if (p_ptr->state.wizard)
			{
				msgf("You do %d (out of %d) damage.", k, m_ptr->hp);
			}

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, &fear, NULL))
			{
				/* Hack -- High-level warriors can spread their attacks out
				 * among weaker foes. -LM-
				 */
				if (((p_ptr->rp.pclass == CLASS_WARRIOR) ||
					 (p_ptr->rp.pclass == CLASS_CHAOS_WARRIOR)) &&
					(p_ptr->lev > 39) && (num < p_ptr->num_blow) &&
					(p_ptr->state.energy_use))
				{
					p_ptr->state.energy_use =
						p_ptr->state.energy_use * num / p_ptr->num_blow;
				}

				mdeath = TRUE;
				break;
			}

			/* Anger the monster */
			if (k > 0) anger_monster(m_ptr);

			touch_zap_player(m_ptr);

			/*
			 * Are we draining it?  A little note: If the monster is
			 * dead, the drain does not work...
			 */
			if (drain_power)
			{
				int max_drain = drain_power * (MAX_VAMPIRIC_DRAIN / 4);
				int drain_left;

				/* Calculate the difference */
				drain_result -= m_ptr->hp;

				/* Did we really hurt it? */
				if (drain_result > 0)
				{
					drain_heal = damroll(drain_power, drain_result / 6);
					drain_left = max_drain - drain_total;

					if (cheat_xtra)
					{
						msgf("Draining left: %d", drain_left);
					}

					if (drain_left > 0)
					{
						/* Cap life gain */
						if (drain_heal > drain_left)
							drain_heal = drain_left;

						/* Add to total */
						drain_total += drain_heal;

						if (drain_msg)
						{
							msgf("Your weapon drains life from %s!",
									   m_name);
							drain_msg = FALSE;
						}

						/* We get to keep some of it! */
						hp_player(drain_heal);
					}
				}
			}

			/* Confusion attack */
			if (p_ptr->state.confusing || do_conf)
			{
				/* Cancel glowing hands */
				if (p_ptr->state.confusing)
				{
					p_ptr->state.confusing = FALSE;
					msgf("Your hands stop glowing.");
					p_ptr->redraw |= (PR_STATUS);
				}

				/* Confuse the monster */
				if (FLAG(r_ptr, RF_NO_CONF))
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags[2] |= RF2_NO_CONF;
					}

					msgf("%^s is unaffected.", m_name);
				}
				else if (randint0(100) < r_ptr->level)
				{
					msgf("%^s is unaffected.", m_name);
				}
				else
				{
					msgf("%^s appears confused.", m_name);
					m_ptr->confused += 10 + randint0(p_ptr->lev) / 5;
				}
			}
			else if (do_tele)
			{
				bool resists_tele = FALSE;

				if (FLAG(r_ptr, RF_RES_TELE))
				{
					if (FLAG(r_ptr, RF_UNIQUE))
					{
						if (m_ptr->ml) r_ptr->r_flags[2] |= RF2_RES_TELE;
						msgf("%^s is unaffected!", m_name);
						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (m_ptr->ml) r_ptr->r_flags[2] |= RF2_RES_TELE;
						msgf("%^s resists!", m_name);
						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					msgf("%^s disappears!", m_name);
					teleport_away(c_ptr->m_idx, 50);
					num = p_ptr->num_blow + 1;	/* Can't hit it anymore! */
					no_extra = TRUE;
				}
			}
			else if (do_poly && cave_floor_grid(c_ptr) &&
					 (randint1(90) > r_ptr->level))
			{
				if (!(FLAG(r_ptr, RF_UNIQUE)) &&
					!(FLAG(r_ptr, RF_BR_CHAO)) &&
					!(FLAG(r_ptr, RF_QUESTOR)))
				{
					if (polymorph_monster(x, y))
					{
						msgf("%^s changes!", m_name);

						/* Hack -- Get new monster */
						m_ptr = &m_list[c_ptr->m_idx];

						/* Oops, we need a different name... */
						monster_desc(m_name, m_ptr, 0, 80);

						/* Hack -- Get new race */
						r_ptr = &r_info[m_ptr->r_idx];

						fear = FALSE;
					}
					else
					{
						msgf("%^s is unaffected.", m_name);
					}
				}
			}

			make_noise(4);
		}

		/* Player misses */
		else
		{
			/* Sound */
			sound(SOUND_MISS);

			/* Message */
			msgf(MSGT_MISS, "You miss %s.", m_name);
			msg_effect(MSG_MISS, m_ptr->r_idx);
		}
	}


	/* Mutations which yield extra 'natural' attacks */
	if (!no_extra)
	{
		if ((p_ptr->muta2 & MUT2_HORNS) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HORNS, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BEAK) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BEAK, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_SCOR_TAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_SCOR_TAIL, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TRUNK) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TRUNK, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TENTACLES) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TENTACLES, &fear, &mdeath);
	}

	/* 
	 * Hack -- delay paralysis (otherwise, consecutive attacks would make
	 * it counterproductive
	 */
	if (ghoul_paral > 0)
	{
		/* Message */
		if (ghoul_hack && (m_ptr->ml))
		{
			msgf("%^s falls asleep!", m_name);
		}

		/* Sleep */
		m_ptr->csleep += 5 * ghoul_paral;
		ghoul_paral = 0;
	}

	/* Hack -- delay fear messages */
	else if (fear && m_ptr->ml)
	{
		flee_message(m_name, m_ptr->r_idx);
	}

	if (drain_total > 0)
	{
		if (one_in_(4)) chg_virtue(V_VITALITY, 1);
	}

	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		int px = p_ptr->px;
		int py = p_ptr->py;

		(void)earthquake(px, py, 10);
	}
}


static void summon_pattern_vortex(int x, int y)
{
	int i;

	/* Find the pattern vortex */
	for (i = 1; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Summon it */
		if (strstr(r_name + r_ptr->name, "Pattern") && (r_ptr->d_char == 'v'))
		{
			if (summon_named_creature(x, y, i, FALSE, FALSE, FALSE))
			{
				msgf("You hear a bell chime.");
			}
		}
	}
}


static bool pattern_seq(int c_x, int c_y, int n_x, int n_y)
{
	cave_type *c1_ptr, *c2_ptr;

	c1_ptr = area(c_x, c_y);
	c2_ptr = area(n_x, n_y);

	if (!cave_pattern_grid(c1_ptr) && !cave_pattern_grid(c2_ptr))
		return TRUE;

	if (c2_ptr->feat == FEAT_PATTERN_START)
	{
		if (!cave_pattern_grid(c1_ptr) &&
			!p_ptr->tim.confused && !p_ptr->tim.stun && !p_ptr->tim.image)
		{
			if (get_check
				("If you start walking the Pattern, you must walk the whole way. Ok? "))
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((c2_ptr->feat == FEAT_PATTERN_OLD) ||
			 (c2_ptr->feat == FEAT_PATTERN_END) ||
			 (c2_ptr->feat == FEAT_PATTERN_XTRA2))
	{
		if (cave_pattern_grid(c1_ptr))
		{
			return TRUE;
		}
		else
		{
			if (get_check("Really step onto the Pattern here? "))
			{
				take_hit(100, "Stepping onto the Pattern");

				if (one_in_(3)) summon_pattern_vortex(n_x, n_y);

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else if ((c2_ptr->feat == FEAT_PATTERN_XTRA1) ||
			 (c1_ptr->feat == FEAT_PATTERN_XTRA1))
	{
		return TRUE;
	}
	else if (c1_ptr->feat == FEAT_PATTERN_START)
	{
		if (cave_pattern_grid(c2_ptr))
			return TRUE;
		else
		{
			if (get_check("Really step off of the Pattern? "))
			{
				take_hit(10, "Stepping off of the Pattern");

				if (one_in_(6)) summon_pattern_vortex(n_x, n_y);

				return TRUE;
			}

			return FALSE;
		}
	}
	else if ((c1_ptr->feat == FEAT_PATTERN_OLD) ||
			 (c1_ptr->feat == FEAT_PATTERN_END) ||
			 (c1_ptr->feat == FEAT_PATTERN_XTRA2))
	{
		if (!cave_pattern_grid(c2_ptr))
		{
			if (get_check("Really step off of the Pattern? "))
			{
				take_hit(100, "Stepping off of the Pattern");

				if (one_in_(2)) summon_pattern_vortex(n_x, n_y);

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!cave_pattern_grid(c1_ptr))
		{
			if (get_check("Really step onto the Pattern here? "))
			{
				take_hit(25, "Stepping onto the Pattern");

				if (one_in_(6)) summon_pattern_vortex(n_x, n_y);

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			byte ok_move = FEAT_PATTERN_START;

			switch (c1_ptr->feat)
			{
				case FEAT_PATTERN_1:
				{
					ok_move = FEAT_PATTERN_2;
					break;
				}
				case FEAT_PATTERN_2:
				{
					ok_move = FEAT_PATTERN_3;
					break;
				}
				case FEAT_PATTERN_3:
				{
					ok_move = FEAT_PATTERN_4;
					break;
				}
				case FEAT_PATTERN_4:
				{
					ok_move = FEAT_PATTERN_1;
					break;
				}
				default:
				{
					if (p_ptr->state.wizard)
						msgf("Funny Pattern walking, %d.",
								   *area(c_x, c_y));

					/* Goof-up */
					return TRUE;
				}
			}

			if ((c2_ptr->feat == ok_move) || (c2_ptr->feat == c1_ptr->feat))
				return TRUE;

			else
			{
				if (!cave_pattern_grid(c2_ptr)
					&& get_check("Really step off of the Pattern? "))
				{
					take_hit(50, "Stepping off of the Pattern");

					if (one_in_(3)) summon_pattern_vortex(n_x, n_y);

					return TRUE;
				}

				else if (cave_pattern_grid(c2_ptr)
						 && get_check("Really stray from the proper path? "))
				{
					take_hit(25, "Walking backwards along the Pattern");

					if (one_in_(5)) summon_pattern_vortex(n_x, n_y);

					return TRUE;
				}

				return FALSE;
			}
		}
	}
}


/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
void move_player(int dir, int do_pickup)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int y, x;

	cave_type *c_ptr;
	pcave_type *pc_ptr;
	monster_type *m_ptr;
	object_type *o_ptr;

	char m_name[80];

	bool p_can_pass_walls = FALSE;
	bool stormbringer = FALSE;
	bool p_cant_pass_fields;

	bool oktomove = TRUE;

	/* Find the result of moving */
	y = py + ddy[dir];
	x = px + ddx[dir];

	/* Do not exit the wilderness-area */
	if (!p_ptr->depth)
	{
		if (!in_bounds2(x, y))
		{
			/* Do not leave the wilderness */
			msgf("You can not leave the wilderness.");
			p_ptr->state.energy_use = 0;
			return;
		}
	}

	/* Examine the destination */
	c_ptr = area(x, y);
	pc_ptr = parea(x, y);

	/* Get the monster */
	m_ptr = &m_list[c_ptr->m_idx];

	/* Access weapon */
	o_ptr = &p_ptr->equipment[EQUIP_WIELD];

	/* Mega-hack */
	if (o_ptr->xtra_name)
	{
		if (streq(quark_str(o_ptr->xtra_name), "'Stormbringer'"))
		{
			stormbringer = TRUE;
		}
	}

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	if (p_ptr->tim.wraith_form || (FLAG(p_ptr, TR_PASS_WALL)))
		p_can_pass_walls = TRUE;

	/* Never walk through permanent features */
	if (cave_perma_grid(c_ptr) && cave_wall_grid(c_ptr))
	{
		p_can_pass_walls = FALSE;
	}

	/* Get passability of field(s) if there */
	p_cant_pass_fields = fields_have_flags(c_ptr, FIELD_INFO_NO_ENTER);

	/* Hack -- attack monsters */
	if (c_ptr->m_idx
		&& (m_ptr->ml || cave_floor_grid(c_ptr) || p_can_pass_walls))
	{
		/* Attack -- only if we can see it OR it is not in a wall */
		if (!is_hostile(m_ptr) &&
			!(p_ptr->tim.confused || p_ptr->tim.image || !m_ptr->ml || p_ptr->tim.stun ||
			  ((p_ptr->muta2 & MUT2_BERS_RAGE) && p_ptr->tim.shero)) &&
			(pattern_seq(px, py, x, y)) &&
			((cave_floor_grid(c_ptr)) || p_can_pass_walls))
		{
			m_ptr->csleep = 0;

			/* Extract monster name (or "it") */
			monster_desc(m_name, m_ptr, 0, 80);

			/* Auto-Recall if possible and visible */
			if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

			/* Track a new monster */
			if (m_ptr->ml) health_track(c_ptr->m_idx);

			/* displace? */
			if (stormbringer && (randint1(1000) > 666))
			{
				py_attack(x, y);
			}
			else if (cave_floor_grid(area(px, py)) ||
					 FLAG(&r_info[m_ptr->r_idx], RF_PASS_WALL))
			{
				msgf("You push past %s.", m_name);
				m_ptr->fy = py;
				m_ptr->fx = px;
				area(px, py)->m_idx = c_ptr->m_idx;
				c_ptr->m_idx = 0;
				update_mon(area(px, py)->m_idx, TRUE);
			}
			else
			{
				msgf("%^s is in your way!", m_name);
				p_ptr->state.energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			py_attack(x, y);
			oktomove = FALSE;
		}
	}

	/* Fields can block movement */
	else if (p_cant_pass_fields)
	{
		msgf("You can't cross that!");
		p_ptr->state.running = 0;
		oktomove = FALSE;
	}

	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	else if ((c_ptr->feat == FEAT_TREES) ||
			 (c_ptr->feat == FEAT_PINE_TREE) || (c_ptr->feat == FEAT_SNOW_TREE))
	{
		oktomove = TRUE;
		if (p_ptr->rp.pclass != CLASS_RANGER) p_ptr->state.energy_use += 10;
	}

	/* Some terrains are hard to move through */
	else if ((c_ptr->feat == FEAT_MOUNTAIN) ||
			 (c_ptr->feat == FEAT_SNOW_MOUNTAIN) ||
			 (c_ptr->feat == FEAT_OBELISK) || (c_ptr->feat == FEAT_BOULDER))
	{
		oktomove = TRUE;
		p_ptr->state.energy_use += 10;
	}

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && is_visible_trap(c_ptr))
	{
		(void)do_cmd_disarm_aux(c_ptr, dir);
		return;
	}

	else if (cave_floor_grid(c_ptr))
	{
		oktomove = TRUE;
	}

	/* Closed door */
	else if (c_ptr->feat == FEAT_CLOSED)
	{
		/* Pass through the door? */
		if (p_can_pass_walls)
		{
			/* Automatically open the door? */
			if (easy_open && !do_cmd_open_aux(x, y))
			{
				oktomove = FALSE;

				/* Disturb the player */
				disturb(FALSE);
			}
		}
		else
		{
			oktomove = FALSE;

			/* Disturb the player */
			disturb(FALSE);

			/* Notice things in the dark */
			if ((pc_ptr->feat != c_ptr->feat) && !player_can_see_grid(pc_ptr))
			{
				msgf("You feel a closed door blocking your way.");
				remember_grid(c_ptr, pc_ptr);
				lite_spot(x, y);
			}

			/* Notice things */
			else
			{
				/* Try to open a door if is there */
				if (easy_open)
				{
					(void)do_cmd_open_aux(x, y);
					return;
				}

				msgf("There is a closed door blocking your way.");

				if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
					p_ptr->state.energy_use = 0;
			}

			/* Sound */
			sound(SOUND_HITWALL);
		}
	}

	/* Player can not walk through "walls" unless in wraith form... */
	else if (!p_can_pass_walls)
	{
		oktomove = FALSE;

		/* Disturb the player */
		disturb(FALSE);

		/* Notice things in the dark */
		if ((pc_ptr->feat != c_ptr->feat) && !player_can_see_grid(pc_ptr))
		{
			msgf(MSGT_HITWALL, "You feel something blocking your way.");
			remember_grid(c_ptr, pc_ptr);
			lite_spot(x, y);
		}
		/* Notice things */
		else
		{
			/* Rubble */
			if (c_ptr->feat == FEAT_RUBBLE)
			{
				msgf(MSGT_HITWALL, "There is rubble blocking your way.");

				if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
					p_ptr->state.energy_use = 0;

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
			}

			/* Jungle */
			else if (c_ptr->feat == FEAT_JUNGLE)
			{
				msgf(MSGT_HITWALL, "The jungle is impassable.");

				if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
					p_ptr->state.energy_use = 0;
			}

			/* Pillar */
			else if (c_ptr->feat == FEAT_PILLAR)
			{
				msgf(MSGT_HITWALL, "There is a pillar blocking your way.");

				if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
					p_ptr->state.energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
				msgf(MSGT_HITWALL, "There is a wall blocking your way.");

				if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
					p_ptr->state.energy_use = 0;
			}
		}

		/* Sound */
		sound(SOUND_HITWALL);
	}

	/* Normal movement */
	if (!pattern_seq(px, py, x, y))
	{
		if (!(p_ptr->tim.confused || p_ptr->tim.stun || p_ptr->tim.image))
		{
			p_ptr->state.energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(FALSE);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		int oy, ox;

		/* Save old location */
		oy = py;
		ox = px;

		/* Process fields under the player. */
		field_hook(area(px, py), FIELD_ACT_PLAYER_LEAVE);

		/* Move the player */
		p_ptr->py = y;
		p_ptr->px = x;

		/* Notice movement */
		Term_move_player();

		if (!p_ptr->depth)
		{
			/* Scroll wilderness */
			p_ptr->wilderness_x = x;
			p_ptr->wilderness_y = y;
			move_wild();
		}

		/* Redraw new spot */
		lite_spot(x, y);

		/* Redraw old spot */
		lite_spot(ox, oy);

		/* Process fields under the player. */
		field_hook(area(x, y), FIELD_ACT_PLAYER_ENTER);

		/* Sound */
		/* sound(SOUND_WALK); */

		/* Check for new panel (redraw map) */
		verify_panel();

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MON_LITE);

		/* Update the monsters */
		p_ptr->update |= (PU_DISTANCE);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Warn about traps */

		/* 
		 * Is the disturb_traps option set and out of detection range?
		 */
		if (disturb_traps && !(pc_ptr->player & GRID_DTCT) &&
			p_ptr->state.detected)
		{
			/* We are out of range */

			msgf("Out of trap detection range.");

			/* Reset the detection flag */
			p_ptr->state.detected = FALSE;

			/* Disturb the player */
			disturb(FALSE);
		}

		/* Spontaneous Searching */
		if ((p_ptr->skills[SKILL_FOS] >= 50) || one_in_(50 - p_ptr->skills[SKILL_FOS]))
		{
			search();
		}

		/* Continuous Searching */
		if (p_ptr->state.searching)
		{
			search();
		}

		/* Handle "objects" */
		carry(do_pickup != always_pickup);

		make_noise(2);
	}
}


/*
 * Hack -- Check for a "known wall" (see below)
 */
static int see_wall(int dir, int x, int y)
{
	pcave_type *pc_ptr;

	feature_type *f_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are "walls" */
	if (!in_boundsp(x, y)) return (TRUE);

	pc_ptr = parea(x, y);

	f_ptr = &f_info[pc_ptr->feat];

	/* Return block-los status */
	return (f_ptr->flags & FF_BLOCK);
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int x, int y)
{
	cave_type *c_ptr;
	pcave_type *pc_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_boundsp(x, y)) return (FALSE);

	c_ptr = area(x, y);
	pc_ptr = parea(x, y);

	/* Memorized grids are always known */
	if (pc_ptr->feat) return (FALSE);

	/* Non-floor grids are unknown */
	if (cave_wall_grid(c_ptr)) return (TRUE);

	/* Viewable door/wall grids are known */
	if (player_can_see_grid(pc_ptr)) return (FALSE);

	/* Default */
	return (TRUE);
}





/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * #@x       1
 * ########### ######
 * 2           #
 * #############
 * #
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 * ...!   ...
 * .o@!   .o.!
 * ...!   ..@!
 * !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * .s
 * @x?
 * #?
 *
 * If they are both seen to be closed, then it is seen that no
 * benefit is gained from moving straight. It is a known corner.
 * To cut the corner, go diagonally, otherwise go straight, but
 * pretend you stepped diagonally into that next location for a
 * full view next time. Conversely, if one of the ? squares is
 * not seen to be closed, then there is a potential choice. We check
 * to see whether it is a potential corner or an intersection/room entrance.
 * If the square two spaces straight ahead, and the space marked with 's'
 * are both blank, then it is a potential corner and enter if find_examine
 * is set, otherwise must stop because it is not a corner.
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };


/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(int dir)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int row, col, deepleft, deepright;
	int i, shortleft, shortright;


	if (!p_ptr->depth)
	{
		/* If in the wilderness - run max 32 squares at a time */
		p_ptr->state.running = 32;
	}
	else
	{
		/* Large number to prevent infinite loops */
		p_ptr->state.running = 1000;
	}

	/* Save the direction */
	p_ptr->run.cur_dir = dir;

	/* Assume running straight */
	p_ptr->run.old_dir = dir;

	/* Assume looking for open area */
	p_ptr->run.open_area = TRUE;

	/* Assume not looking for breaks */
	p_ptr->run.break_right = FALSE;
	p_ptr->run.break_left = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	/* Extract cycle index */
	i = chome[dir];

	/* Check for nearby wall */
	if (see_wall(cycle[i + 1], px, py))
	{
		p_ptr->run.break_left = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i + 1], col, row))
	{
		p_ptr->run.break_left = TRUE;
		deepleft = TRUE;
	}

	/* Check for nearby wall */
	if (see_wall(cycle[i - 1], px, py))
	{
		p_ptr->run.break_right = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i - 1], col, row))
	{
		p_ptr->run.break_right = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (p_ptr->run.break_left && p_ptr->run.break_right)
	{
		/* Not looking for open area */
		p_ptr->run.open_area = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				p_ptr->run.old_dir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				p_ptr->run.old_dir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], col, row))
		{
			if (shortleft && !shortright)
			{
				p_ptr->run.old_dir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				p_ptr->run.old_dir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int prev_dir, new_dir, check_dir = 0;
	int row, col;
	int i, max, inv;
	int option = 0, option2 = 0;

	object_type *o_ptr;

	cave_type *c_ptr;
	pcave_type *pc_ptr;

	/* Where we came from */
	prev_dir = p_ptr->run.old_dir;

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;


	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		if (!in_boundsp(col, row)) continue;

		/* Access grid */
		c_ptr = area(col, row);
		pc_ptr = parea(col, row);


		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		OBJ_ITT_START (c_ptr->o_idx, o_ptr)
		{
			/* Visible object */
			if (o_ptr->info & (OB_SEEN)) return (TRUE);
		}
		OBJ_ITT_END;

		/* Visible traps abort running */
		if (is_visible_trap(c_ptr)) return TRUE;

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (pc_ptr->feat)
		{
			bool notice = TRUE;

			/* Examine the terrain */
			switch (c_ptr->feat)
			{
				case FEAT_FLOOR:
				case FEAT_SECRET:
				case FEAT_MAGMA:
				case FEAT_QUARTZ:

				case FEAT_WALL_EXTRA:
				case FEAT_WALL_INNER:
				case FEAT_WALL_OUTER:
				case FEAT_WALL_SOLID:
				case FEAT_PERM_EXTRA:
				case FEAT_PERM_INNER:
				case FEAT_PERM_OUTER:
				case FEAT_PERM_SOLID:

				case FEAT_SAND:
				case FEAT_SALT:
				case FEAT_WET_MUD:
				case FEAT_DRY_MUD:
				case FEAT_FLOOR_TILE:
				case FEAT_FLOOR_WOOD:
				case FEAT_PEBBLES:
				case FEAT_SOLID_LAVA:
				case FEAT_SHAL_WATER:
				case FEAT_DIRT:
				case FEAT_GRASS:
				case FEAT_TREE_WATER:

				case FEAT_TREES:
				case FEAT_MOUNTAIN:
				case FEAT_SNOW_MOUNTAIN:
				case FEAT_PINE_TREE:
				case FEAT_SNOW_TREE:
				case FEAT_OBELISK:
				case FEAT_PILLAR:

				case FEAT_BUSH:
				case FEAT_DEAD_BUSH:
				case FEAT_GRASS_LONG:
				case FEAT_ROCK_GEN:
				case FEAT_ROCK_SNOW:
				case FEAT_TREE_GEN:
				case FEAT_TREE_SNOW:
				case FEAT_SNOW:
				case FEAT_JUNGLE:
				{
					/* Ignore */
					notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_LAVA:
				case FEAT_SHAL_LAVA:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_FIRE)))
						 notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_ACID:
				case FEAT_SHAL_ACID:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_ACID)))
						 notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_SWAMP:
				case FEAT_SHAL_SWAMP:
				{
					/* Ignore */
					if (p_ptr->tim.invuln || (FLAG(p_ptr, TR_IM_POIS)))
						notice = FALSE;

					/* Done */
					break;
				}


				case FEAT_DEEP_WATER:
				case FEAT_OCEAN_WATER:
				{
					/* Ignore */
					if (FLAG(p_ptr, TR_FEATHER)) notice = FALSE;

					/* Done */
					break;
				}

					/* Open doors */
				case FEAT_OPEN:
				case FEAT_BROKEN:
				{
					/* Option -- ignore */
					if (find_ignore_doors) notice = FALSE;

					/* Done */
					break;
				}

					/* Stairs */
				case FEAT_LESS:
				case FEAT_MORE:
				{
					/* Option -- ignore */
					if (find_ignore_stairs) notice = FALSE;

					/* Done */
					break;
				}
			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors */
		if (inv || cave_floor_grid(c_ptr))
		{
			/* Looking for open area */
			if (p_ptr->run.open_area)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (p_ptr->run.open_area)
			{
				if (i < 0)
				{
					/* Break to the right */
					p_ptr->run.break_right = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					p_ptr->run.break_left = TRUE;
				}
			}
		}
	}


	/* Looking for open area */
	if (p_ptr->run.open_area)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			if (!in_bounds2(col, row)) continue;

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!see_wall(new_dir, px, py))

			{
				/* Looking to break right */
				if (p_ptr->run.break_right)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (p_ptr->run.break_left)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			if (!in_bounds2(col, row)) continue;

			/* Access grid */
			c_ptr = area(col, row);

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!see_wall(new_dir, px, py))

			{
				/* Looking to break left */
				if (p_ptr->run.break_left)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (p_ptr->run.break_right)
				{
					return (TRUE);
				}
			}
		}
	}


	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			p_ptr->run.cur_dir = option;

			/* No other options */
			p_ptr->run.old_dir = option;
		}

		/* Two options, examining corners */
		else if (find_examine && !find_cut)
		{
			/* Primary option */
			p_ptr->run.cur_dir = option;

			/* Hack -- allow curving */
			p_ptr->run.old_dir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, col, row) || !see_wall(check_dir, col, row))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (find_examine &&
					see_nothing(option, col, row) &&
					see_nothing(option2, col, row))
				{
					p_ptr->run.cur_dir = option;
					p_ptr->run.old_dir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				p_ptr->run.cur_dir = option2;
				p_ptr->run.old_dir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				p_ptr->run.cur_dir = option;
				p_ptr->run.old_dir = option2;
			}
		}
	}


	/* About to hit a known wall, stop */
	if (see_wall(p_ptr->run.cur_dir, px, py))
	{
		return (TRUE);
	}


	/* Failure */
	return (FALSE);
}


/*
 * Take one step along the current "run" path
 */
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		/* Hack -- do not start silly run */
		if (see_wall(dir, p_ptr->px, p_ptr->py))
		{
			/* Message */
			msgf("You cannot run in that direction.");

			/* Disturb */
			disturb(FALSE);

			/* Done */
			return;
		}

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(FALSE);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	p_ptr->state.running--;

	/* Take time */
	p_ptr->state.energy_use = 100;

	/* Move the player, using the "pickup" flag */
	move_player(p_ptr->run.cur_dir, FALSE);
}
