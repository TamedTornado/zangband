/* File: borg3.c */

/* Purpose: Object and Spell routines for the Borg -BEN- */

#include "angband.h"


#ifdef ALLOW_BORG

#include "zborg1.h"
#include "zborg3.h"


/*
 * This file helps the Borg analyze "objects" and "shops", and to
 * deal with objects and spells.
 */




/*
 * Spell info
 */

borg_magic borg_magics[8][4][8];	/* Spell info, by realm/book/what */
borg_mind borg_minds[MINDCRAFT_MAX];

/*
 * Hack -- help analyze the magic
 *
 * The comments yield the "name" of the spell or prayer.
 *
 * Also, the leading letter in the comment indicates how we use the
 * spell or prayer, if at all, using "A" for "attack", "D" for "call
 * light" and "detection", "E" for "escape", "H" for healing, "O" for
 * "object manipulation", and "F" for "terrain feature manipulation",
 * plus "!" for entries that can soon be handled.
 */

static byte borg_magic_method[8][4][8] =
{
	{							/* 0 Realm 0 -- Non spell caster */
	 {							/* Book0... (sval 0) */
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ },
	 {							/* Book1... (sval 1) */
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ },
	 {							/* Book0... (sval 2) */
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ },
	 {							/* Book3... (sval 3) */
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /* ! "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ ,
	  BORG_MAGIC_ICK /*   "(blank)" */ }
	 },							/* end of realm 0 */

	{							/* 1 Life Realm */
	 {							/* Common Prayers (sval 0) */
	  BORG_MAGIC_NOP /*   "Detect Evil" */ ,
	  BORG_MAGIC_NOP /*   "Cure Light Wounds" */ ,
	  BORG_MAGIC_NOP /*   "Bless" */ ,
	  BORG_MAGIC_NOP /* H "Remove Fear" */ ,
	  BORG_MAGIC_NOP /* D "Call Light" */ ,
	  BORG_MAGIC_NOP /* D "Find Traps" */ ,
	  BORG_MAGIC_NOP /* D "Cure Medium Wounds" */ ,
	  BORG_MAGIC_NOP /*   "Satisfy Hunger" */ ,},

	 {							/* High Mass (sval 1) */
	  BORG_MAGIC_NOP /*   "Remove Curse" */ ,
	  BORG_MAGIC_NOP /* E "Cure Poison" */ ,
	  BORG_MAGIC_NOP /* H "Cure Crit Wounds" */ ,
	  BORG_MAGIC_NOP /*   "See Inv" */ ,
	  BORG_MAGIC_AIM /*   "Holy Orb" */ ,
	  BORG_MAGIC_NOP /* H "PFE" */ ,
	  BORG_MAGIC_NOP /*   "Healing" */ ,
	  BORG_MAGIC_NOP /*   "Rune of Protection" */ ,},

	 {							/* Book of the Unicorn (sval 2) */
	  BORG_MAGIC_NOP /* H "Exorcism" */ ,
	  BORG_MAGIC_NOP /* A "Dispel Curse" */ ,
	  BORG_MAGIC_NOP /* H "Disp Undead and Demon" */ ,
	  BORG_MAGIC_NOP /*   "Day of Dove" */ ,
	  BORG_MAGIC_NOP /*   "Dispel Evil" */ ,
	  BORG_MAGIC_NOP /* D "Banishment" */ ,
	  BORG_MAGIC_NOP /* H "Holy Word" */ ,
	  BORG_MAGIC_NOP /*   "Warding True" */ },

	 {							/* Blessings of the Grail (sval 3) */
	  BORG_MAGIC_NOP /*   "Heroism" */ ,
	  BORG_MAGIC_NOP /* ! "Prayer" */ ,
	  BORG_MAGIC_NOP /* H "Bless Weapon" */ ,
	  BORG_MAGIC_NOP /* ! "Restoration" */ ,
	  BORG_MAGIC_NOP /*   "Healing True" */ ,
	  BORG_MAGIC_OBJ /* ! "Holy Vision" */ ,
	  BORG_MAGIC_NOP /*   "Divine Intervent" */ ,
	  BORG_MAGIC_NOP /*   "Holy Invuln" */ }

	 },							/* endof Life Realm */

	{							/*2. Sorcery Realm */
	 {							/* Beginners Handbook (sval 0) */
	  BORG_MAGIC_NOP /*   "Detect Monster" */ ,
	  BORG_MAGIC_NOP /*   "Phase Door" */ ,
	  BORG_MAGIC_NOP /*   "Detect Doors & Traps" */ ,
	  BORG_MAGIC_NOP /*   "Light Area" */ ,
	  BORG_MAGIC_AIM /*   "Confuse Monster" */ ,
	  BORG_MAGIC_NOP /*   "Teleport Self" */ ,
	  BORG_MAGIC_NOP /*   "Sleep Monster" */ ,
	  BORG_MAGIC_OBJ /*   "Recharging" */ },

	 {							/* Master Sorcery (sval 1) */
	  BORG_MAGIC_NOP /*   "Magic Map" */ ,
	  BORG_MAGIC_OBJ /*   "Ident" */ ,
	  BORG_MAGIC_AIM /*   "Slow Monster" */ ,
	  BORG_MAGIC_NOP /*   "Mass Sleep " */ ,
	  BORG_MAGIC_AIM /*   "Teleport Away" */ ,
	  BORG_MAGIC_NOP /*   "Haste Self" */ ,
	  BORG_MAGIC_NOP /*   "Detection True" */ ,
	  BORG_MAGIC_OBJ /*   "*ID*" */ },

	 {							/* Pattern Sorcery (sval 2) */
	  BORG_MAGIC_NOP /*   "Detect Obj" */ ,
	  BORG_MAGIC_NOP /*   "Detect Enchant" */ ,
	  BORG_MAGIC_ICK /*   "Charm Mon" */ ,
	  BORG_MAGIC_AIM /*   "Dimension Door" */ ,
	  BORG_MAGIC_NOP /*   "Sense Minds" */ ,
	  BORG_MAGIC_NOP /*   "Self Knowledge" */ ,
	  BORG_MAGIC_NOP /*   "Teleport Level" */ ,
	  BORG_MAGIC_NOP /*   "Word of Recall" */ },

	 {							/* Grimoir of Power (sval 3) */
	  BORG_MAGIC_AIM /*   "Stasis" */ ,
	  BORG_MAGIC_ICK /*   "Telekinesis" */ ,
	  BORG_MAGIC_ICK /*   "Explosive Rune" */ ,
	  BORG_MAGIC_NOP /*   "Clairvoyance" */ ,
	  BORG_MAGIC_OBJ /*   "*Enchant Weap" */ ,
	  BORG_MAGIC_OBJ /*   "*Enchant Armor" */ ,
	  BORG_MAGIC_ICK /*   "Alchemy" */ ,
	  BORG_MAGIC_NOP /*   "GOI" */ }
	 },							/* End of Sorcery Realm */

	{							/* 3 Nature Realm */
	 {							/* Call of the Wild (sval 0) */
	  BORG_MAGIC_NOP /*   "Detect Creature" */ ,
	  BORG_MAGIC_NOP /*   "First Aid" */ ,
	  BORG_MAGIC_NOP /* ! "Detect Door" */ ,
	  BORG_MAGIC_NOP /*   "Foraging" */ ,
	  BORG_MAGIC_NOP /*   "Daylight" */ ,
	  BORG_MAGIC_AIM /*   "Animal Taming" */ ,
	  BORG_MAGIC_NOP /*   "Resist Environment" */ ,
	  BORG_MAGIC_NOP /*   "Cure Wound&Poison" */ },
	 {							/* Nature Mastery (sval 1) */
	  BORG_MAGIC_AIM /* ! "Stone to Mud" */ ,
	  BORG_MAGIC_AIM /* ! "Lightning Bolt" */ ,
	  BORG_MAGIC_NOP /*   "Nature Awareness" */ ,
	  BORG_MAGIC_AIM /*   "Frost Bolt" */ ,
	  BORG_MAGIC_AIM /*   "Ray of Sunlight" */ ,
	  BORG_MAGIC_NOP /*   "Entangle" */ ,
	  BORG_MAGIC_ICK /*   "Summon Animals" */ ,
	  BORG_MAGIC_NOP /*   "Herbal Healing" */ },
	 {							/* Nature Gifts (sval 2) */
	  BORG_MAGIC_NOP /* ! "Door Building" */ ,
	  BORG_MAGIC_NOP /*   "Stair Building" */ ,
	  BORG_MAGIC_NOP /* ! "Stone Skin" */ ,
	  BORG_MAGIC_NOP /*   "Resistance True" */ ,
	  BORG_MAGIC_NOP /*   "Animal Friend" */ ,
	  BORG_MAGIC_OBJ /*   "Stone Tell" */ ,
	  BORG_MAGIC_NOP /*   "Wall of Stone" */ ,
	  BORG_MAGIC_OBJ /*   "Protect From Corros." */ },
	 {							/* Natures Wrath (sval 3) */
	  BORG_MAGIC_NOP /* ! "Earthquake" */ ,
	  BORG_MAGIC_NOP /*   "Whirlwind" */ ,
	  BORG_MAGIC_AIM /* ! "Blizzard" */ ,
	  BORG_MAGIC_AIM /*   "Lightning" */ ,
	  BORG_MAGIC_AIM /*   "Whirpool" */ ,
	  BORG_MAGIC_NOP /*   "Call Sunlight" */ ,
	  BORG_MAGIC_OBJ /*   "Elemental Brand" */ ,
	  BORG_MAGIC_NOP /*   "Natures Wrath" */ }
	 },							/* end of Natural realm  */

	{							/* 4.Chaos Realm */
	 {							/* Sign of Chaos... (sval 0) */
	  BORG_MAGIC_AIM /* "Magic Missile" */ ,
	  BORG_MAGIC_NOP /* "Trap/Door Dest" */ ,
	  BORG_MAGIC_NOP /* "Flash of Light" */ ,
	  BORG_MAGIC_NOP /* "Touch of Conf" */ ,
	  BORG_MAGIC_NOP /* "ManaBurst" */ ,
	  BORG_MAGIC_AIM /* "Fire Bolt" */ ,
	  BORG_MAGIC_AIM /* "Fist of Force" */ ,
	  BORG_MAGIC_NOP /* "Teleport" */ },
	 {							/* Chaos Mastery... (sval 1) */
	  BORG_MAGIC_ICK /*   "Wonder" */ ,
	  BORG_MAGIC_AIM /*   "Chaos Bolt" */ ,
	  BORG_MAGIC_NOP /*   "Sonic Boom" */ ,
	  BORG_MAGIC_AIM /*   "Doom Beam" */ ,
	  BORG_MAGIC_AIM /*   "Fire Ball" */ ,
	  BORG_MAGIC_AIM /*   "Teleport Other" */ ,
	  BORG_MAGIC_NOP /*   "Word of Dest" */ ,
	  BORG_MAGIC_NOP /*   "Invoke Logrus" */ },
	 {							/* Chaos Channels (sval 2) */
	  BORG_MAGIC_AIM /*   "Polymorph Other" */ ,
	  BORG_MAGIC_NOP /*   "Chain Lightn" */ ,
	  BORG_MAGIC_OBJ /*   "Arcane Binding" */ ,
	  BORG_MAGIC_AIM /*   "Disintegration" */ ,
	  BORG_MAGIC_NOP /*   "Alter Reality" */ ,
	  BORG_MAGIC_ICK /*   "Polymorph Self" */ ,
	  BORG_MAGIC_ICK /*   "Chaos Brinding" */ ,
	  BORG_MAGIC_ICK /*   "Summon Demon" */ },
	 {							/* Armageddon Tome (sval 3) */
	  BORG_MAGIC_AIM /*   "Gravity Beam" */ ,
	  BORG_MAGIC_AIM /*   "Meteor Swarm" */ ,
	  BORG_MAGIC_NOP /*   "Flame Strike" */ ,
	  BORG_MAGIC_NOP /*   "Call Chaos" */ ,
	  BORG_MAGIC_AIM /*   "Magic Rocket" */ ,
	  BORG_MAGIC_AIM /*   "Mana Storm" */ ,
	  BORG_MAGIC_AIM /*   "Breath Logrus" */ ,
	  BORG_MAGIC_NOP /*   "Call Void" */ }
	 },							/* end of Chaos Realm */

	{							/* 5. Death Realm */
	 {							/* Black Prayers (sval 0) */
	  BORG_MAGIC_NOP /* ! "Detect Unlife" */ ,
	  BORG_MAGIC_AIM /*   "Malediction" */ ,
	  BORG_MAGIC_NOP /* ! "Detect Evil" */ ,
	  BORG_MAGIC_AIM /*   "Stinking Cloud" */ ,
	  BORG_MAGIC_AIM /*   "Black Sleep" */ ,
	  BORG_MAGIC_NOP /*   "Resist Poison" */ ,
	  BORG_MAGIC_AIM /*   "Horrify" */ ,
	  BORG_MAGIC_AIM /*   "Enslave Undead" */ },
	 {							/* Black Mass (sval 1) */
	  BORG_MAGIC_AIM /* ! "Orb of Entropy" */ ,
	  BORG_MAGIC_AIM /*   "Nether Bolt" */ ,
	  BORG_MAGIC_NOP /*   "Terror" */ ,
	  BORG_MAGIC_AIM /*   "Vamp Drain" */ ,
	  BORG_MAGIC_OBJ /*   "Poison Brand" */ ,
	  BORG_MAGIC_NOP /*   "Disp Good" */ ,
	  BORG_MAGIC_WHO /*   "Genocide" */ ,
	  BORG_MAGIC_NOP /*   "Restore Life" */ },
	 {							/* Black Channels (sval 2) */
	  BORG_MAGIC_NOP /* ! "Berserk" */ ,
	  BORG_MAGIC_NOP /*   "Invoke Spirits" */ ,
	  BORG_MAGIC_AIM /* ! "Dark Bolt" */ ,
	  BORG_MAGIC_NOP /*   "Battle Frenzy" */ ,
	  BORG_MAGIC_AIM /*   "Vamp True" */ ,
	  BORG_MAGIC_OBJ /*   "Vamp Brand" */ ,
	  BORG_MAGIC_AIM /*   "Dark Storm" */ ,
	  BORG_MAGIC_NOP /*   "Mass Genocide" */ },
	 {							/* Necronomicon (sval 3) */
	  BORG_MAGIC_AIM /* ! "Death Ray" */ ,
	  BORG_MAGIC_ICK /*   "Raise the Dead" */ ,
	  BORG_MAGIC_OBJ /* ! "Esoteria" */ ,
	  BORG_MAGIC_NOP /*   "Word of Death" */ ,
	  BORG_MAGIC_NOP /*   "Evocation" */ ,
	  BORG_MAGIC_AIM /*   "Hellfire" */ ,
	  BORG_MAGIC_NOP /*   "Omnicide" */ ,
	  BORG_MAGIC_NOP /*   "Wraithform" */ }
	 },							/* end of Death Realm */

	{							/* 6 Trump Realm */
	 {							/* Conjuring and Tricks (sval 0) */
	  BORG_MAGIC_NOP /* ! "Phase Door" */ ,
	  BORG_MAGIC_AIM /*   "Mind Blast" */ ,
	  BORG_MAGIC_ICK /*   "Shuffle" */ ,
	  BORG_MAGIC_ICK /*   "Reset Recall" */ ,
	  BORG_MAGIC_NOP /*   "Teleport Self" */ ,
	  BORG_MAGIC_AIM /*   "Dimension Door" */ ,
	  BORG_MAGIC_NOP /*   "Trump Spying" */ ,
	  BORG_MAGIC_AIM /*   "Teleport Away" */ },
	 {							/* Deck of Many Things (sval 1) */
	  BORG_MAGIC_ICK /* ! "Trump Object" */ ,
	  BORG_MAGIC_ICK /* ! "Trump Animal" */ ,
	  BORG_MAGIC_NOP /*   "Phantasmal Servant" */ ,
	  BORG_MAGIC_ICK /*   "Trump Monster" */ ,
	  BORG_MAGIC_ICK /*   "Conjure Elemental" */ ,
	  BORG_MAGIC_NOP /*   "Teleport Level" */ ,
	  BORG_MAGIC_NOP /*   "Word of Recall" */ ,
	  BORG_MAGIC_NOP /*   "Banish" */ },
	 {							/* Trumps of Doom (sval 2) */
	  BORG_MAGIC_ICK /* ! "Joker Card" */ ,
	  BORG_MAGIC_ICK /*   "Trump Spiders" */ ,
	  BORG_MAGIC_ICK /*   "T. Reptiles" */ ,
	  BORG_MAGIC_ICK /*   "T. Houdns" */ ,
	  BORG_MAGIC_ICK /*   "T. Branding" */ ,
	  BORG_MAGIC_ICK /*   "Living Trump" */ ,
	  BORG_MAGIC_ICK /*   "Death Dealing" */ ,
	  BORG_MAGIC_ICK /*   "T. Cyberdemon" */ },
	 {							/* Five Aces (sval 3) */
	  BORG_MAGIC_NOP /* ! "T. Divination" */ ,
	  BORG_MAGIC_OBJ /*   "T. Lore" */ ,
	  BORG_MAGIC_ICK /*   "T. Undead" */ ,
	  BORG_MAGIC_ICK /*   "T. Dragon" */ ,
	  BORG_MAGIC_ICK /*   "Mass Trump" */ ,
	  BORG_MAGIC_ICK /*   "T. Demon" */ ,
	  BORG_MAGIC_ICK /*   "T. Ancient Dragon " */ ,
	  BORG_MAGIC_ICK /*   "T. Greater Undead" */ }
	 },							/* end of Trump Realm */

	{							/* 7 Arcane Realm */
	 {							/* Cantrips (sval 0) */
	  BORG_MAGIC_AIM /* ! "Zap" */ ,
	  BORG_MAGIC_AIM /*   "Wiz Lock" */ ,
	  BORG_MAGIC_NOP /*   "Det Invis" */ ,
	  BORG_MAGIC_NOP /*   "Det Mon" */ ,
	  BORG_MAGIC_NOP /*   "Blink" */ ,
	  BORG_MAGIC_NOP /*   "Light Area" */ ,
	  BORG_MAGIC_AIM /*   "Trap/Door Dest" */ ,
	  BORG_MAGIC_NOP /*   "Cure Light Wounds" */ },
	 {							/* Minor Arcana (sval 1) */
	  BORG_MAGIC_NOP /* ! "Det Door/Trap" */ ,
	  BORG_MAGIC_NOP /* ! "Phlogiston" */ ,
	  BORG_MAGIC_NOP /*   "Det Treasure" */ ,
	  BORG_MAGIC_NOP /*   "Det Enchant" */ ,
	  BORG_MAGIC_NOP /*   "Det Object" */ ,
	  BORG_MAGIC_NOP /*   "Cure Poison" */ ,
	  BORG_MAGIC_NOP /*   "Resist Cold" */ ,
	  BORG_MAGIC_NOP /*   "Resist Fre" */ },
	 {							/* Major Arcana (sval 2) */
	  BORG_MAGIC_NOP /* ! "Resist Elec" */ ,
	  BORG_MAGIC_NOP /*   "Resist Acid" */ ,
	  BORG_MAGIC_NOP /* ! "Cure Med Wounds" */ ,
	  BORG_MAGIC_NOP /*   "Teleport" */ ,
	  BORG_MAGIC_AIM /*   "Stone to Mud" */ ,
	  BORG_MAGIC_AIM /*   "Ray of Light" */ ,
	  BORG_MAGIC_NOP /*   "Satisfy Hunger" */ ,
	  BORG_MAGIC_NOP /*   "See Invis" */ },
	 {							/* Manual of Mastery (sval 3) */
	  BORG_MAGIC_OBJ /* ! "Recharge" */ ,
	  BORG_MAGIC_NOP /*   "Teleport Level" */ ,
	  BORG_MAGIC_OBJ /* ! "Ident" */ ,
	  BORG_MAGIC_AIM /*   "Teleport Away" */ ,
	  BORG_MAGIC_AIM /*   "Elemental Ball" */ ,
	  BORG_MAGIC_NOP /*   "Detection" */ ,
	  BORG_MAGIC_NOP /*   "Word of Recall" */ ,
	  BORG_MAGIC_NOP /*   "Clairvoyance" */ }
	 }							/* end of Arcane Realm */


};



/*
 * Hack -- help analyze the magic
 *
 * The comments yield the "name" of the spell or prayer.
 *
 * Also, the leading letter in the comment indicates how we use the
 * spell or prayer, if at all, using "A" for "attack", "D" for "call
 * light" and "detection", "E" for "escape", "H" for healing, "O" for
 * "object manipulation", "F" for "terrain feature manipulation",
 * "X" for "never use this", and "!" for "soon to be handled".
 *
 * The value indicates how much we want to know the spell/prayer.  A
 * rating of zero indicates that the spell/prayer is useless, and should
 * never be learned or used.  A rating from 1 to 49 indicates that the
 * spell/prayer is worth some experience to use once, so we should study
 * (and use) it when we get bored in town.  A rating from 50 to 99 means
 * that the spell/prayer should be learned as soon as possible (and used
 * when bored).
 *
 * XXX XXX XXX Verify ratings.
 */

static byte borg_magic_rating[8][4][8] =
{
	{							/* Null Realm */
	 {							/* Book0... (sval 0) */
	  0 /* ! "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ },
	 {							/* Book1... (sval 1) */
	  0 /*   "(blank)" */ ,
	  0 /* ! "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ },
	 {							/* Book2... (sval 2) */
	  0 /*   "(blank)" */ ,
	  0 /* ! "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ },
	 {							/* Book3... (sval 3) */
	  0 /* ! "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /* ! "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ ,
	  0 /*   "(blank)" */ }
	 },							/* end of Null Realm */

	{							/* Life Realm */
	 {							/* Common Prayers (sval 0) */
	  85 /*   "Detect Evil" */ ,
	  55 /*   "Cure Light Wounds" */ ,
	  85 /*   "Bless" */ ,
	  35 /*   "Remove Fear" */ ,
	  35 /*   "Call Light" */ ,
	  75 /*   "Find Traps & Doors" */ ,
	  65 /*   "Cure Medium Wounds" */ ,
	  85 /*   "Satisfy Hunger" */ },
	 {							/* High Mass (sval 1) */
	  65 /*   "Remove Curse" */ ,
	  65 /*   "Cure Poison" */ ,
	  85 /*   "Cure Crit Wounds" */ ,
	  55 /*   "See Invis" */ ,
	  95 /*   "Holy Orb" */ ,
	  85 /*   "Prot/Evil" */ ,
	  65 /*   "Heal 300" */ ,
	  55 /*   "Glyph" */ },
	 {							/* Book of the Unicorn (sval 2) */
	  65 /*   "Exorcism" */ ,
	  65 /*   "Dispel Curse" */ ,
	  55 /*   "Dispel Demon" */ ,
	  0 /*   "Day of Dove" */ ,
	  65 /*   "Dispel Evil" */ ,
	  55 /*   "Banishment" */ ,
	  65 /*   "Holy Word" */ ,
	  55 /*   "Warding True" */ },
	 {							/* Blessings of the Grail (sval 3) */
	  55 /*   "Heroism" */ ,
	  65 /*   "Prayer" */ ,
	  45 /*   "Bless Weapon" */ ,
	  55 /*   "Restoration" */ ,
	  65 /*   "Healing 2000" */ ,
	  55 /*   "Holy Vision" */ ,
	  55 /*   "Divine Intervent" */ ,
	  55 /*   "Holy Invuln" */ }
	 },							/* end of Life Magic */

	{							/* Sorcery Realm */
	 {							/* Beginners Handbook (sval 0) */
	  95 /*   "Detect Monsters" */ ,
	  85 /*   "Phase Door" */ ,
	  65 /*   "Detect Door" */ ,
	  85 /*   "Light Area" */ ,
	  75 /*   "Confuse Monster" */ ,
	  75 /*   "Teleport Selft" */ ,
	  65 /*   "Sleep Monster" */ ,
	  65 /*   "Recharging" */ },
	 {							/* Master Sorcery (sval 1) */
	  55 /*   "Magic Map" */ ,
	  85 /*   "Identify" */ ,
	  55 /*   "Slow Monster" */ ,
	  65 /*   "Mass Sleep" */ ,
	  95 /*   "Teleport Away" */ ,
	  55 /*   "Haste Self" */ ,
	  85 /*   "Detection True" */ ,
	  75 /*   "*Identify*" */ },
	 {							/* Pattern Sorcery (sval 2) */
	  55 /*   "Detect Obj/Treasure" */ ,
	  55 /*   "Detect Enchantment" */ ,
	  75 /*   "Charm Monster" */ ,
	  65 /*   "Dimension Door" */ ,
	  65 /*   "Sense Minds" */ ,
	  0 /*   "Self Knowledge" */ ,
	  65 /*   "Teleport Level" */ ,
	  65 /*   "Word of Recall" */ },
	 {							/* Grimoir of Power (sval 3) */
	  55 /*   "Stasis" */ ,
	  0 /*   "Telekinesis" */ ,
	  0 /*   "Explosive Rune" */ ,
	  65 /*   "Clairvoyance" */ ,
	  55 /*   "Enchant Weap" */ ,
	  55 /*   "Enchant Armour" */ ,
	  1 /*   "Alchemy" */ ,
	  95 /*   "GOI" */ }
	 },							/* end of Sorcery Realm */

	{							/* 3 Nature Realm */
	 {							/* Call of the Wild (sval 0) */
	  65 /*   "Detect Creature" */ ,
	  65 /*   "First Aid" */ ,
	  55 /* ! "Detect Door" */ ,
	  75 /*   "Foraging" */ ,
	  75 /*   "Daylight" */ ,
	  55 /*   "Animal Taming" */ ,
	  75 /*   "Resist Environment" */ ,
	  65 /*   "Cure Wound&Poison" */ },
	 {							/* Nature Mastery (sval 1) */
	  55 /* ! "Stone to Mud" */ ,
	  65 /* ! "Lightning Bolt" */ ,
	  65 /*   "Nature Awareness" */ ,
	  65 /*   "Frost Bolt" */ ,
	  65 /*   "Ray of Sunlight" */ ,
	  65 /*   "Entangle" */ ,
	  65 /*   "Summon Animals" */ ,
	  65 /*   "Herbal Healing" */ },
	 {							/* Nature Gifts (sval 2) */
	  65 /* ! "Door Building" */ ,
	  45 /*   "Stair Building" */ ,
	  65 /* ! "Stone Skin" */ ,
	  65 /*   "Resistance True" */ ,
	  55 /*   "Animal Friend" */ ,
	  65 /*   "Stone Tell" */ ,
	  45 /*   "Wall of Stone" */ ,
	  45 /*   "Protect From Corros." */ },
	 {							/* Natures Wrath (sval 3) */
	  65 /* ! "Earthquake" */ ,
	  65 /*   "Whirlwind" */ ,
	  65 /* ! "Blizzard" */ ,
	  65 /*   "Lightning" */ ,
	  65 /*   "Whirpool" */ ,
	  65 /*   "Call Sunlight" */ ,
	  45 /*   "Elemental Brand" */ ,
	  65 /*   "Natures Wrath" */ }
	 },							/* end of Natural realm  */

	{							/* 4.Chaos Realm */
	 {							/* Sign of Chaos... (sval 0) */
	  95 /* "Magic Missile" */ ,
	  65 /* "Trap/Door Dest" */ ,
	  75 /* "Flash of Light" */ ,
	  55 /* "Touch of Conf" */ ,
	  65 /* "ManaBurst" */ ,
	  65 /* "Fire Bolt" */ ,
	  65 /* "Fist of Force" */ ,
	  75 /* "Teleport" */ },
	 {							/* Chaos Mastery... (sval 1) */
	  5 /*   "Wonder" */ ,
	  65 /*   "Chaos Bolt" */ ,
	  65 /*   "Sonic Boom" */ ,
	  65 /*   "Doom Beam" */ ,
	  65 /*   "Fire Ball" */ ,
	  65 /*   "Teleport Other" */ ,
	  65 /*   "Word of Dest" */ ,
	  55 /*   "Invoke Logrus" */ },
	 {							/* Chaos Channels (sval 2) */
	  45 /*   "Polymorph Other" */ ,
	  65 /*   "Chain Lightn" */ ,
	  65 /*   "Arcane Binding" */ ,
	  65 /*   "Disintegration" */ ,
	  55 /*   "Alter Reality" */ ,
	  5 /*   "Polymorph Self" */ ,
	  55 /*   "Chaos Binding" */ ,
	  55 /*   "Summon Demon" */ },
	 {							/* Armageddon Tome (sval 3) */
	  65 /*   "Gravity Beam" */ ,
	  65 /*   "Meteor Swarm" */ ,
	  65 /*   "Flame Strike" */ ,
	  65 /*   "Call Chaos" */ ,
	  75 /*   "Magic Rocket" */ ,
	  75 /*   "Mana Storm" */ ,
	  65 /*   "Breath Logrus" */ ,
	  65 /*   "Call Void" */ }
	 },							/* end of Chaos Realm */

	{							/* 5. Death Realm */
	 {							/* Black Prayers (sval 0) */
	  65 /* ! "Detect Unlife" */ ,
	  75 /*   "Maledition" */ ,
	  75 /* ! "Detect Evil" */ ,
	  75 /*   "Stinking Cloud" */ ,
	  65 /*   "Black Sleep" */ ,
	  65 /*   "Resist Poison" */ ,
	  65 /*   "Horrify" */ ,
	  65 /*   "Enslave Undead" */ },
	 {							/* Black Mass (sval 1) */
	  70 /* ! "Orb of Entropy" */ ,
	  65 /*   "Nether Bolt" */ ,
	  50 /*   "Terror" */ ,
	  65 /*   "Vamp Drain" */ ,
	  55 /*   "Poison Brand" */ ,
	  65 /*   "Disp Good" */ ,
	  65 /*   "Genocide" */ ,
	  65 /*   "Restore Life" */ },
	 {							/* Black Channels (sval 2) */
	  65 /* ! "Berserk" */ ,
	  65 /*   "Invoke Spirits" */ ,
	  65 /* ! "Dark Bolt" */ ,
	  85 /*   "Battle Frenzy" */ ,
	  65 /*   "Vamp True" */ ,
	  65 /*   "Vamp Brand" */ ,
	  65 /*   "Dark Storm" */ ,
	  65 /*   "Mass Genocide" */ },
	 {							/* Necronomicon (sval 3) */
	  65 /* ! "Death Ray" */ ,
	  65 /*   "Raise the Dead" */ ,
	  75 /* ! "Esoteria" */ ,
	  65 /*   "Word of Death" */ ,
	  65 /*   "Evocation" */ ,
	  65 /*   "Hellfire" */ ,
	  65 /*   "Omnicide" */ ,
	  55 /*   "Wraithform" */ }
	 },							/* end of Death Realm */


	{							/* Trump Realm */
	 {							/* Trump Magic (sval 0) */
	  95 /* ! "Phase Door" */ ,
	  85 /* ! "Mind Blast " */ ,
	  0 /*   "Shuffle" */ ,
	  0 /*   "Reset Recall" */ ,
	  75 /*   "Tlelport Self" */ ,
	  65 /*   "Dimension Door " */ ,
	  65 /*   "Trump Spying " */ ,
	  70 /*   "Teleport Away " */ },
	 {							/* Deck of Many Things (sval 1) */
	  0 /* ! "Trump Object " */ ,
	  0 /* ! "Trump animal " */ ,
	  85 /*   "Phantasmal Servant " */ ,
	  0 /*   "Trump Monster " */ ,
	  0 /*   "Conjure Elemental " */ ,
	  50 /*   "Teleport Level " */ ,
	  65 /*   "Word of recall " */ ,
	  65 /*   "Banishment" */ },
	 {							/* Trump of Doom (sval 2) */
	  0 /* ! "Joker Card " */ ,
	  0 /*   "Trump Spiders " */ ,
	  0 /*   "Trump Reptiles " */ ,
	  0 /*   "Trump Hounds " */ ,
	  0 /*   "Trump Branding " */ ,
	  0 /*   "Living Trump " */ ,
	  0 /*   "Death Dealing " */ ,
	  0 /*   "Trump Cyberdemon " */ },
	 {							/* Five Aces (sval 3) */
	  0 /* ! "Trump Divination " */ ,
	  0 /*   "Trump Lore " */ ,
	  0 /*   "Trump Undead " */ ,
	  0 /*   "Trump Dragon " */ ,
	  0 /*   "Mass Trump " */ ,
	  0 /*   "Trump Demon " */ ,
	  0 /*   "Trump Ancient Dragon " */ ,
	  0 /*   "Trump Greater Undead " */ }
	 },							/* end of Trump Realm */

	{							/* 7 Arcane Realm */
	 {							/* Cantrips (sval 0) */
	  85 /* ! "Zap" */ ,
	  85 /*   "Wiz Lock" */ ,
	  75 /*   "Det Invis" */ ,
	  75 /*   "Det Mon" */ ,
	  75 /*   "Blink" */ ,
	  75 /*   "Light Area" */ ,
	  85 /*   "Trap/Door Dest" */ ,
	  75 /*   "Cure Light Wounds" */ },
	 {							/* Minor Arcana (sval 1) */
	  75 /* ! "Det Door/Trap" */ ,
	  75 /* ! "Phlogiston" */ ,
	  75 /*   "Det Treasure" */ ,
	  75 /*   "Det Enchant" */ ,
	  75 /*   "Det Object" */ ,
	  75 /*   "Cure Poison" */ ,
	  75 /*   "Resist Cold" */ ,
	  75 /*   "Resist Fre" */ },
	 {							/* Major Arcana (sval 2) */
	  75 /* ! "Resist Elec" */ ,
	  75 /*   "Resist Acid" */ ,
	  75 /* ! "Cure Med Wounds" */ ,
	  75 /*   "Teleport" */ ,
	  85 /*   "Stone to Mud" */ ,
	  85 /*   "Ray of Light" */ ,
	  75 /*   "Satisfy Hunger" */ ,
	  75 /*   "See Invis" */ },
	 {							/* Manual of Mastery (sval 3) */
	  75 /* ! "Recharge" */ ,
	  75 /*   "Teleport Level" */ ,
	  85 /* ! "Ident" */ ,
	  85 /*   "Teleport Away" */ ,
	  70 /*   "Elemental Ball" */ ,
	  75 /*   "Detection" */ ,
	  75 /*   "Word of Recall" */ ,
	  75 /*   "Clairvoyance" */ }
	 }							/* end of Arcane Realm */

};

/*
 * Return the slot that items of the given type are wielded into
 *
 * Note that "rings" are tough because there are two slots
 *
 * Returns "-1" if the item cannot (or should not) be wielded
 */
int borg_wield_slot(list_item *l_ptr)
{
	if ((l_ptr->tval == TV_SWORD) ||
		(l_ptr->tval == TV_POLEARM) ||
		(l_ptr->tval == TV_HAFTED) ||
		(l_ptr->tval == TV_DIGGING)) return (EQUIP_WIELD);

	if ((l_ptr->tval == TV_DRAG_ARMOR) ||
		(l_ptr->tval == TV_HARD_ARMOR) ||
		(l_ptr->tval == TV_SOFT_ARMOR)) return (EQUIP_BODY);

	if (l_ptr->tval == TV_SHIELD) return (EQUIP_ARM);

	if ((l_ptr->tval == TV_CROWN) ||
		(l_ptr->tval == TV_HELM)) return (EQUIP_HEAD);

	if (l_ptr->tval == TV_BOW) return (EQUIP_BOW);

	if (l_ptr->tval == TV_RING) return (EQUIP_LEFT);

	if (l_ptr->tval == TV_AMULET) return (EQUIP_NECK);

	if (l_ptr->tval == TV_LITE) return (EQUIP_LITE);

	if (l_ptr->tval == TV_CLOAK) return (EQUIP_OUTER);

	if (l_ptr->tval == TV_GLOVES) return (EQUIP_HANDS);

	if (l_ptr->tval == TV_BOOTS) return (EQUIP_FEET);

	/* No slot available */
	return (-1);
}

/*
 * Find the index of the object_kind with the given tval and sval
 */
object_kind *borg_get_kind(int tval, int sval)
{
	int k;
	int num = 0;
	object_kind *kb_ptr = &k_info[0];

	/* Look for it */
	for (k = 1; k < z_info->k_max; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Require correct tval */
		if (k_ptr->tval != tval) continue;

		/* Found a match */
		if (k_ptr->sval == sval) return (k_ptr);

		/* Ignore illegal items */
		if (sval != SV_ANY) continue;

		/* Apply the randomizer */
		if (!one_in_(++num)) continue;

		/* Use this value */
		kb_ptr = k_ptr;
	}

	/* Failure? */
	if (sval != SV_ANY)
	{
		/* Oops */
		borg_note_fmt("No object (%d,%d)", tval, sval);
	}

	return (kb_ptr);
}


/*
 * Find the slot of an item with the given tval/sval, if available.
 * Given multiple choices, choose the item with the largest "pval".
 * Given multiple choices, choose the smallest available pile.
 */
list_item *borg_slot(int tval, int sval)
{
	int i;

	object_kind *k_ptr;

	/* Scan the pack */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip un-aware items */
		if (!l_ptr->k_idx) continue;

		k_ptr = &k_info[l_ptr->k_idx];

		/* Require correct tval */
		if (k_ptr->tval != tval) continue;

		/* Require correct sval */
		if (k_ptr->sval != sval) continue;

		/* Hack - Prefer the first match, it is sorted nicely already */
		return (l_ptr);
	}

	/* Done */
	return (NULL);
}

/*
 * Return the slot of an item with the given tval/sval, if available.
 * The first available is returned.  The search is started from <from>
 * This way with repeated calls the second pile of an item can be found.
 */
int borg_slot_from(int tval, int sval, int from)
{
	int i;

	/* Scan the pack */
	for (i = from; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip un-aware items */
		if (!l_ptr->k_idx) continue;

		/* Continue looking for the right tval */
		if (l_ptr->tval != tval) continue;

		/* Require correct sval */
		if (k_info[l_ptr->k_idx].sval != sval) continue;

		/* This is what was looked for */
		return (i);
	}

	/* Object not found */
	return (-1);
}

/*
 * Get the index of an item so we can send commands to the game
 */
int look_up_index(list_item *l_ptr)
{
	int i;

	/* Scan inventory */
	for (i = 0; i < inven_num; i++)
	{
		if (&inventory[i] == l_ptr) return (i);
	}

	/* Paranoia */
	borg_oops("Trying to find invalid object!");

	return (-1);
}


/*
 * Hack -- refuel a torch with the minimal torch
 */
bool borg_refuel_torch(void)
{
	int slot, b_slot = -1, fuel = 9999;
	list_item *l_ptr;

	/* Must first wield before one can refuel */
	if (!equipment[EQUIP_LITE].k_idx) return (FALSE);

 	/* Must wield torch */
	if (k_info[equipment[EQUIP_LITE].k_idx].sval != SV_LITE_TORCH) return (FALSE);

	/* Cast phlogiston */
	if (borg_spell_fail(REALM_ARCANE, 1, 1, 40)) return (TRUE);

	/* Look for the minimal torch */
	for (slot = 0; slot < inven_num; slot++)
	{
		l_ptr = &inventory[slot];

		/* Must be a light */
		if (l_ptr->tval != TV_LITE) continue;

		/* Must be a torch */
		if (k_info[l_ptr->k_idx].sval != SV_LITE_TORCH) continue;

		/* Ignore torches with the most fuel */
		if (l_ptr->timeout >= fuel) continue;

		/* Is this an ego_torch? */
		if (borg_obj_is_ego_art(l_ptr)) continue;

		/* My favorite torch */
		b_slot = slot;
		fuel = l_ptr->timeout;
	}

	/* None available */
	if (b_slot == -1) return (FALSE);

	/* Log the message */
	borg_note_fmt("# Refueling with %s.", inventory[b_slot].o_name);

	/* Perform the action */
	borg_keypress('F');
	borg_keypress(I2A(b_slot));

	/* Success */
	return (TRUE);
}


/*
 * Hack -- refuel a lantern
 */
bool borg_refuel_lantern(void)
{
	int slot, b_slot = -1, fuel = 14999;
	list_item *l_ptr;

	/* Must first wield before one can refuel */
	if (!equipment[EQUIP_LITE].k_idx) return (FALSE);

 	/* Must wield lantern */
	if (k_info[equipment[EQUIP_LITE].k_idx].sval != SV_LITE_LANTERN)
		return (FALSE);

	/* Cast phlogiston */
	if (borg_spell_fail(REALM_ARCANE, 1, 1, 40)) return (TRUE);

	/* Loop through the inventory backwards */
	for (slot = inven_num - 1; slot >= 0; slot--)
	{
		l_ptr = &inventory[slot];

		/* Maybe fuel with a Lantern? */
		if (l_ptr->tval == TV_LITE &&
			k_info[l_ptr->k_idx].sval == SV_LITE_LANTERN)
		{
			/* Ignore lanterns with the most fuel */
			if (l_ptr->timeout >= fuel) continue;

			/* My favorite lantern */
			b_slot = slot;
			fuel = l_ptr->timeout;
		}
		else
		{
			/* Maybe fuel with a flask? */
			if (l_ptr->tval == TV_FLASK)
			{
				/* Get out of the loop */
				break;
			}
		}
	}

	/* b_slot holds best lantern, slot holds flask, let's see if there is one */
	if (b_slot == -1 && slot == -1) return (FALSE);

	/* Found no lantern but a flask */
	if (b_slot == -1) b_slot = slot;

	/* Log the message */
	borg_note_fmt("# Refueling with %s.", inventory[b_slot].o_name);

	/* Perform the action */
	borg_keypress('F');
	borg_keypress(I2A(b_slot));

	/* Success */
	return (TRUE);
}




/*
 * Hack -- attempt to eat the given food (by sval)
 */
bool borg_eat_food(int sval)
{
	list_item *l_ptr;

	/* Look for that food */
	l_ptr = borg_slot(TV_FOOD, sval);

	/* None available */
	if (!l_ptr) return (FALSE);

	/* Log the message */
	borg_note_fmt("# Eating %s.", l_ptr->o_name);

	/* Perform the action */
	borg_keypress('E');
	borg_keypress(I2A(look_up_index(l_ptr)));

	/* Success */
	return (TRUE);
}


static s32b when_last_quaff = 0;

/*
 * Quaff a potion of cure critical wounds.  This is a special case
 *   for several reasons.
 *   1) it is usually the only healing potion we have on us
 *   2) we should try to conserve a couple for when we really need them
 *   3) if we are burning through them fast we should probably teleport out of
 *      the fight.
 *   4) When it is the only/best way out of danger, drink away
  */
bool borg_quaff_crit(bool no_check)
{
	if (no_check)
	{
		if (borg_quaff_potion(SV_POTION_CURE_CRITICAL))
		{
			when_last_quaff = borg_t;
			return (TRUE);
		}
		return (FALSE);
	}

	/* Save the last two for when we really need them */
	if (bp_ptr->able.ccw < 2) return FALSE;

	/* Avoid drinking CCW twice in a row */
	if (when_last_quaff > (borg_t - 4) &&
		when_last_quaff <= borg_t && (randint0(100) < 75))
		return FALSE;

	if (borg_quaff_potion(SV_POTION_CURE_CRITICAL))
	{
		when_last_quaff = borg_t;
		return (TRUE);
	}
	return (FALSE);
}


/*
 * Hack -- attempt to quaff the given potion (by sval)
 */
bool borg_quaff_potion(int sval)
{
	list_item *l_ptr;

	/* Look for that potion */
	l_ptr = borg_slot(TV_POTION, sval);

	/* None available */
	if (!l_ptr) return (FALSE);

	/* Log the message */
	borg_note_fmt("# Quaffing %s.", l_ptr->o_name);

	/* Perform the action */
	borg_keypress('q');
	borg_keypress(I2A(look_up_index(l_ptr)));

	/* Success */
	return (TRUE);
}

/*
 * Hack -- attempt to quaff an unknown potion
 */
bool borg_quaff_unknown(void)
{
	int i;

	/* Scan the pack */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Require correct tval */
		if (l_ptr->tval != TV_POTION) continue;

		/* Skip aware items */
		if (l_ptr->k_idx) continue;

		/* Log the message */
		borg_note_fmt("# Quaffing unknown potion %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('q');
		borg_keypress(I2A(i));

		/* Success */
		return (TRUE);
	}

	/* None available */
	return (FALSE);
}

/*
 * Hack -- attempt to read an unknown scroll
 */
bool borg_read_unknown(void)
{
	int i;
	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Scan the pack */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip aware items */
		if (l_ptr->k_idx) continue;

		/* Require correct tval */
		if (l_ptr->tval != TV_SCROLL) continue;

		/* Not when dark */
		if (!(mb_ptr->flags & MAP_GLOW) && !bp_ptr->cur_lite) return (FALSE);

		/* Blind or Confused */
		if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

		/* Log the message */
		borg_note_fmt("# Reading unknown scroll %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('r');
		borg_keypress(I2A(i));

		/* Success */
		return (TRUE);
	}

	/* None available */
	return (FALSE);
}


/*
 * Hack -- attempt to eat an unknown potion.  This is done in emergencies.
 */
bool borg_eat_unknown(void)
{
	int i;

	/* Scan the pack */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip aware items */
		if (l_ptr->k_idx) continue;

		/* Require correct tval */
		if (l_ptr->tval != TV_FOOD) continue;

		/* Log the message */
		borg_note_fmt("# Eating unknown mushroom %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('E');
		borg_keypress(I2A(i));

		/* Success */
		return (TRUE);
	}

	/* None available */
	return (FALSE);
}

/*
 * Hack -- attempt to use an unknown staff.  This is done in emergencies.
 */
bool borg_use_unknown(void)
{
	int i;

	/* Scan the pack */
	for (i = 0; i < inven_num; i++)
	{
		list_item *l_ptr = &inventory[i];

		/* Skip aware items */
		if (l_ptr->k_idx) continue;

		/* Require correct tval */
		if (l_ptr->tval != TV_STAFF) continue;

		/* Log the message */
		borg_note_fmt("# Using unknown Staff %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('u');
		borg_keypress(I2A(i));

		/* Success */
		return (TRUE);
	}

	/* None available */
	return (FALSE);
}

/* Check if the given scroll (by sval) can be read */
bool borg_read_scroll_fail(int sval)
{
	/* Dark */
	if (!map_loc(c_x, c_y)->flags & MAP_GLOW &&
		!bp_ptr->cur_lite) return (FALSE);

	/* Blind or Confused */
	if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

	/* Is the scroll available? */
	if (!borg_slot(TV_SCROLL, sval)) return (FALSE);

	/* The borg has the scroll and can read it too */
	return (TRUE);
}

/* Attempt to read the given scroll (by sval) */
bool borg_read_scroll(int sval)
{
	list_item *l_ptr;
	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Dark */
	if (!(mb_ptr->flags & MAP_GLOW) && !bp_ptr->cur_lite) return (FALSE);

	/* Blind or Confused */
	if (bp_ptr->status.blind || bp_ptr->status.confused) return (FALSE);

	/* Look for that scroll */
	l_ptr = borg_slot(TV_SCROLL, sval);

	/* None available */
	if (!l_ptr) return (FALSE);

	/* Log the message */
	borg_note_fmt("# Reading %s.", l_ptr->o_name);

	/* Perform the action */
	borg_keypress(ESCAPE);
	borg_keypress(ESCAPE);
	borg_keypress('r');
	borg_keypress(I2A(look_up_index(l_ptr)));

	/* reset recall depth in dungeon? */
	if ((sval == SV_SCROLL_WORD_OF_RECALL) &&
		(bp_ptr->depth < bp_ptr->max_depth) && bp_ptr->depth)
	{
		/* Do not reset Depth */
		borg_keypress('n');
	}

	/* Success */
	return (TRUE);
}


/* Take an item and makes a fail check on it */
bool borg_use_item_fail(list_item *l_ptr, bool risky)
{
	int chance, lev;

	/* Extract the item level */
	lev = k_info[l_ptr->k_idx].level;

	/* Base chance of success */
	chance = bp_ptr->skill_dev;

	/* Confusion hurts skill */
	if (bp_ptr->status.confused) chance = chance / 2;
	
	/* Do you feel lucky, punk? */
	if (risky)
	{
		/* Calculate the chance for this item */
		chance = chance - lev / 3;
	}
	else
	{
		/* Calculate the chance for this item */
		chance = chance - lev / 2;
	}

	/* Is this item usable? */
	if (chance < USE_DEVICE) return (FALSE);

	/* Success */
	return (TRUE);
}


/* To zap a rod or not */
static bool borg_rod_aux(int sval, bool zap, bool fail)
{
	int slot;
	list_item *l_ptr;

	/* Look for that rod */
	slot = borg_slot_from(TV_ROD, sval, 0);

	/* None available */
	if (slot == -1) return (FALSE);

	l_ptr = &inventory[slot];

	/* Still charging */
	if (l_ptr->timeout == l_ptr->number) return (FALSE);

	/* Can we zap this rod */
	if (fail && !borg_use_item_fail(l_ptr, FALSE)) return (FALSE);

	/* Do we want to zap it? */
	if (zap)
	{
		/* Log the message */
		borg_note_fmt("# Zapping %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('z');
		borg_keypress(I2A(slot));
	}

	/* Success */
	return (TRUE);
}

/* Does the borg have this rod? */
bool borg_equips_rod(int sval)
{
	return (borg_rod_aux(sval, FALSE, FALSE));
}

/* Does the borg have this rod and will be able to zap it? */
bool borg_equips_rod_fail(int sval)
{
	return (borg_rod_aux(sval, FALSE, TRUE));
}

/* Let's zap this rod if possible  */
bool borg_zap_rod(int sval)
{
	return (borg_rod_aux(sval, TRUE, FALSE));
}


/*
 * Hack -- attempt to use the requested wand (by sval).
 * Wands with unknown charges can also be tried if they are not {empty}
 * If (fail) is set then do a fail check.
 * If (aim) is set then aim the wand.
 */
static bool borg_wand_aux(int sval, bool aim, bool fail)
{
	list_item *l_ptr;
	int slot = 0;

	/* Look for that wand */
	slot = borg_slot_from(TV_WAND, sval, slot);

	/* Search the inventory until the right wand with charges is found */
	while (slot != -1)
	{
		l_ptr = &inventory[slot];

		/* Accept this wand if it may have charges */
		if (borg_obj_known_p(l_ptr))
		{
			/* Identified with charges */
			if (l_ptr->pval) break;
		}
		else
		{
			/* unidentified and not inscribed as empty */
			if (!strstr(l_ptr->o_name, "{empty}")) break;
		}

		/* Look for the next wand */
		slot = borg_slot_from(TV_WAND, sval, slot + 1);
	}

	/* No wand found */
	if (slot == -1) return (FALSE);

	/* Can we aim this wand */
	if (fail && !borg_use_item_fail(l_ptr, FALSE)) return (FALSE);

	/* Aim the wand */
	if (aim)
	{
		/* Log the message */
		borg_note_fmt("# Aiming %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('a');
		borg_keypress(I2A(slot));
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- attempt to aim the given (charged) wand (by sval)
 */
bool borg_aim_wand(int sval)
{
	/* aim that wand without a fail check */
	return (borg_wand_aux(sval, TRUE, FALSE));
}


/*
 * Hack -- attempt to aim the given (charged) wand (by sval)
 */
bool borg_equips_wand_fail(int sval)
{
	/* Search for that wand with a fail check */
	return (borg_wand_aux(sval, FALSE, TRUE));
}


/*
 * Hack -- attempt to use the requested staff (by sval).
 * Staffs with unknown charges can also be tried if they are not {empty}
 * This is ok to do because if the staff is empty the effect (a wasted turn)
 * is the same as a failure to use the staff.
 * If (fail) is set then do a fail check.
 * If (use) is set then use the staff.
 */
static bool borg_staff_aux(int sval, bool use, bool fail)
{
	list_item *l_ptr;
	int slot = 0;

	/* Look for that staff */
	slot = borg_slot_from(TV_STAFF, sval, slot);

	/* Search the inventory until the right staff with charges is found */
	while (slot != -1)
	{
		l_ptr = &inventory[slot];

		/* Accept this staff if it may have charges */
		if (borg_obj_known_p(l_ptr))
		{
			/* Identified with charges */
			if (l_ptr->pval) break;
		}
		else
		{
			/* unidentified and not inscribed as empty */
			if (!strstr(l_ptr->o_name, "{empty}")) break;
		}

		/* Look for the next staff */
		slot = borg_slot_from(TV_STAFF, sval, slot + 1);
	}

	/* No staff found */
	if (slot == -1) return (FALSE);

	/* Do the fail check */
	if (fail)
	{
		if (sval == SV_STAFF_TELEPORTATION ||
			sval == SV_STAFF_DESTRUCTION)
		{
			/* Take more risk if you want to teleport or destruct */
			if (!borg_use_item_fail(l_ptr, TRUE)) return (FALSE);
		}
		else
		{
			/* Do not take more risks for other staffs */
			if (!borg_use_item_fail(l_ptr, FALSE)) return (FALSE);
		}
	}

	/* Use the staff */
	if (use)
	{
		/* Log the message */
		borg_note_fmt("# Using %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('u');
		borg_keypress(I2A(slot));
	}

	/* Success */
	return (TRUE);
}

/* Attempt to use the requested staff (by sval) */
bool borg_use_staff(int sval)
{
	/* Use the staff (if available) without fail check */
	return borg_staff_aux(sval, TRUE, FALSE);
}

/* Attempt to use the staff (by sval) and make a fail check on it. */
bool borg_use_staff_fail(int sval)
{
	/* Use the staff with fail check */
	return borg_staff_aux(sval, TRUE, TRUE);
}

/* Checks staff (by sval) and makes a fail check on it. */
bool borg_equips_staff_fail(int sval)
{
	/* Do not use the staff, just do the fail check */
	return borg_staff_aux(sval, FALSE, TRUE);
}

/* Checks staff (by sval) and without a fail check. */
bool borg_equips_staff(int sval)
{
	/* Do not use the staff, just do the fail check */
	return borg_staff_aux(sval, FALSE, FALSE);
}

/*
 * This function checks if the item is an artifact
 * that can be activated according to your skill.
 * if real_use is TRUE then there is also a check if the
 * borg can use the artifact right now.
 */
bool borg_check_artifact(list_item *l_ptr, bool real_use)
{
	/* Skip empty items */
	if (!l_ptr || !l_ptr->k_idx) return (FALSE);

	/* Skip non-artifacts */
	if (!(KN_FLAG(l_ptr, TR_INSTA_ART))) return (FALSE);

	/* Is this an activatable item? */
	if (!(KN_FLAG(l_ptr, TR_ACTIVATE))) return (FALSE);

	/* Can we activate this artifact */
	if (!borg_use_item_fail(l_ptr, FALSE)) return (FALSE);

	if (!real_use) return (TRUE);

	/* Check charge */
	if (l_ptr->timeout) return (FALSE);
	
	/* We got what we need */
	return (TRUE);
}

/*
 * Hack -- attempt to use the given artifact
 * Doesn't work because I don't know how to find out the activation.
 * It is possible to do this by name (Galadriel, etc) but that leaves
 * out the randarts.
 */
bool borg_activate_artifact(int name1, bool secondary)
{
	int slot,
		act;

	/* Check the equipment */
	for (slot = 0; slot < equip_num; slot++)
	{
		list_item *l_ptr = &equipment[slot];

		/* Is this item an artifact that can be activated now? */
		if (!borg_check_artifact(l_ptr, TRUE)) continue;

		/* 
		 * Find out what activation is, but not like this
		 * act = p_ptr->equipment[slot].activate;
		 * With act = 0 no activation will be found
		 */
		act = 0;

		/* Is this a predefined artifact with the right activation? */
		if (act < 128 || act - 128 != name1) continue;

		/* Log the message */
		borg_note_fmt("# Activating artifact %s.", l_ptr->o_name);

		/* Perform the action */
		borg_keypress('A');
		borg_keypress(I2A(slot));

		/* Jewel also gives Recall */
		if (act - 128 == ART_THRAIN)
		{
			/* probably some spaces missing */
			borg_keypress(' ');

			if (secondary == FALSE)
			{
				borg_keypress('n');
			}
			else
			{
				borg_keypress('y');
			}
		}

		/* Success */
		return (TRUE);
	}

	/* The artifact is not in the equipment */
	return (FALSE);
}


static void borg_dimension_door(void)
{
	int x1, y1, x2, y2;

	/* Follow Dim Door syntax */
	borg_keypress(' ');

	/* Report a little bit */
	borg_note_fmt
		("# Targetting Landing Zone (%d,%d)", dim_door_x, dim_door_y);

	/* Determine "path" */
	x1 = c_x;
	y1 = c_y;
	x2 = dim_door_x;
	y2 = dim_door_y;

	/* Move to the location (diagonals) */
	for (; (y1 < y2) && (x1 < x2); y1++, x1++) borg_keypress('3');
	for (; (y1 < y2) && (x1 > x2); y1++, x1--) borg_keypress('1');
	for (; (y1 > y2) && (x1 < x2); y1--, x1++) borg_keypress('9');
	for (; (y1 > y2) && (x1 > x2); y1--, x1--) borg_keypress('7');

	/* Move to the location */
	for (; y1 < y2; y1++) borg_keypress('2');
	for (; y1 > y2; y1--) borg_keypress('8');
	for (; x1 < x2; x1++) borg_keypress('6');
	for (; x1 > x2; x1--) borg_keypress('4');

	/* Select the target */
	borg_keypress(' ');
}

/* Returns the mana cost of a spell, assuming that the borg can cast it */
byte borg_spell_mana(int realm, int book, int spell)
{
	byte power;

	/* basic cost of the spell */
	power = borg_magics[realm][book][spell].power;

	/* If this is a chaos spell and the borg has a chaos patron */
	if (realm == REALM_CHAOS && FLAG(p_ptr, TR_PATRON))
	{
		/* Reduce the spell cost */
		power = (2 * power + 2) / 3;
	}

	/* Tell the world */
	return (power);
}

/* Combines the legality check with the cost */
static bool borg_mana_legal_fail(int realm, int book, int spell, int fail, byte *cost)
{
	/* Is this spell castable with the fail_check? */
	if (!borg_spell_legal_fail(realm, book, spell, fail)) return (FALSE);

	/* Find out the cost */
	*cost = borg_spell_mana(realm, book, spell);

	/* Success */
	return (TRUE);
}
	

/* This function returns the amount of reserve mana */
int borg_reserve_mana(void)
{
	byte cost;

	/* Don't bother with reserve mana if you can't have spells */
	if (borg_class == CLASS_WARRIOR) return (0);

	/* Low level spell casters should not worry about this */
	if (bp_ptr->lev < 20) return (0);

	/* Special case for Mindcrafters */
	if (borg_class == CLASS_MINDCRAFTER)
	{
		/* This borg has dimension door */
		if (bp_ptr->lev >= 40) return (3 * borg_minds[MIND_MINOR_DISP].power);

		/* Telekinetic Wave */
		if (bp_ptr->msp > 100) return (borg_minds[MIND_TELE_WAVE].power);

		/* Two teleports */
		if (bp_ptr->msp > 50) return (2 * borg_minds[MIND_MAJOR_DISP].power);

		/* One teleport */
		if (bp_ptr->msp > 12) return (borg_minds[MIND_MAJOR_DISP].power);

		/* One phase door */
		if (bp_ptr->msp > 4) return (borg_minds[MIND_MINOR_DISP].power);

		/* Puny! */
		return (0);
	}

	/*
	 * I created these values spell by spell.  If there are multiple realms
	 * carrying a spell then Trump goes first and Arcane goes last as Trump
	 * has low values and Arcane high
	 * Teleport away is not listed because it is covered by the reserve for
	 * teleport spells.
	 */


	/* Multiple Dimension Doors */
	if (borg_mana_legal_fail(REALM_TRUMP, 0, 5, 5, &cost)) return (3 * cost);
	if (borg_mana_legal_fail(REALM_SORCERY, 2, 3, 5, &cost)) return (3 * cost);

	/* Dimension Door */
	if (borg_mana_legal_fail(REALM_TRUMP, 0, 5, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_SORCERY, 2, 3, 15, &cost)) return (cost);

	/* Teleport Level */
	if (borg_mana_legal_fail(REALM_TRUMP, 1, 5, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_ARCANE, 3, 1, 15, &cost)) return (cost);
	
	/* Mass teleport away */
	if (borg_mana_legal_fail(REALM_DEATH, 3, 4, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_LIFE, 2, 5, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_TRUMP, 1, 7, 15, &cost)) return (cost);

	/* Multiple Teleports */
	if (borg_mana_legal_fail(REALM_TRUMP, 0, 4, 5, &cost)) return (2 * cost);
	if (borg_mana_legal_fail(REALM_SORCERY, 0, 5, 5, &cost)) return (2 * cost);
	if (borg_mana_legal_fail(REALM_CHAOS, 1, 7, 5, &cost)) return (2 * cost);
	if (borg_mana_legal_fail(REALM_ARCANE, 2, 3, 5, &cost)) return (2 * cost);

	/* Teleport */
	if (borg_mana_legal_fail(REALM_TRUMP, 0, 4, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_SORCERY, 0, 5, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_CHAOS, 1, 7, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_ARCANE, 2, 3, 15, &cost)) return (cost);

	/* Stair Building */
	if (borg_mana_legal_fail(REALM_NATURE, 2, 1, 15, &cost)) return (cost);

	/* Phase Door? */
	if (borg_mana_legal_fail(REALM_TRUMP, 0, 0, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_SORCERY, 0, 1, 15, &cost)) return (cost);
	if (borg_mana_legal_fail(REALM_ARCANE, 0, 4, 15, &cost)) return (cost);

	/* No spell available */
	return (0);
}


/*
 * This function determines if a given spell is allowed to be cast
 * with regards to reserve_mana
 */
static bool borg_reserve_allow(int realm, int book, int what)
{
	/* Are you dipping into reserve mana? */
	if (bp_ptr->csp - borg_spell_mana(realm, book, what) >= borg_reserve_mana())
	{
		/* Plenty of mana so it is OK */
		return (TRUE);
	}

	switch (realm)
	{
		case REALM_LIFE:
		{
			/* Banishment spells ok */
			if (book == 2 && what == 5) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_SORCERY:
		{
			/* Phase spells ok */
			if (book == 0 && what == 1) return (TRUE);

			/* Teleport spells ok */
			if (book == 0 && what == 5) return (TRUE);

			/* Teleport Away ok */
			if (book == 1 && what == 4) return (TRUE);

			/* Dimension Door spells ok */
			if (book == 2 && what == 3) return (TRUE);

			/* Teleport Level spells ok */
			if (book == 2 && what == 6) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_NATURE:
		{
			/* Stair Building spells ok */
			if (book == 2 && what == 1) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_CHAOS:
		{
			/* Teleport spells ok */
			if (book == 0 && what == 7) return (TRUE);

			/* Teleport Away ok */
			if (book == 1 && what == 5) return (TRUE);

			/* Alter Reality ok */
			if (book == 2 && what == 4) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_DEATH:
		{
			/* Evocation spells ok */
			if (book == 3 && what == 4) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_TRUMP:
		{
			/* Phase spells ok */
			if (book == 0 && what == 0) return (TRUE);

			/* Teleport spells ok */
			if (book == 0 && what == 4) return (TRUE);

			/* Dimension Door spells ok */
			if (book == 0 && what == 5) return (TRUE);

			/* Teleport Away ok */
			if (book == 0 && what == 7) return (TRUE);

			/* Teleport Level spells ok */
			if (book == 1 && what == 5) return (TRUE);

			/* Teleport Level spells ok */
			if (book == 1 && what == 7) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		case REALM_ARCANE:
		{
			/* Phase spells ok */
			if (book == 0 && what == 4) return (TRUE);

			/* Teleport spells ok */
			if (book == 2 && what == 3) return (TRUE);

			/* Satisfy Hunger OK */
			if (book == 2 && what == 6) return (TRUE);

			/* Teleport Level spells ok */
			if (book == 3 && what == 1) return (TRUE);

			/* Teleport Away ok */
			if (book == 3 && what == 3) return (TRUE);

			/* others are rejected */
			return (FALSE);
		}
		default:
		{
			borg_oops("Unknown Realm used in borg_reserve_allow");
			return(0);
		}
	}
}


/*
 * Determine if borg can cast a given spell (when fully rested)
 */
bool borg_spell_legal(int realm, int book, int what)
{
	borg_magic *as = &borg_magics[realm][book][what];

	/* The borg must be able to "cast" spells this realm */
	if (!borg_has_realm(realm)) return (FALSE);

	/* Make sure we have this realm book */
	if (amt_book[realm][book] <= 0) return (FALSE);

	/* The spell must be "known" */
	if (as->status < BORG_MAGIC_TEST) return (FALSE);

	/* The spell must be affordable (when rested) */
	if (borg_spell_mana(realm, book, what) > bp_ptr->msp) return (FALSE);

	/* Not if locked down */
	if (FLAG(bp_ptr, TR_NO_MAGIC)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Determine if borg can cast a given spell (right now)
 */
bool borg_spell_okay(int realm, int book, int what)
{
	map_block *mb_ptr = map_loc(c_x, c_y);

	/* Dark */
	if (!(mb_ptr->flags & MAP_GLOW) && !bp_ptr->cur_lite) return (FALSE);

	/* Require ability (when rested) */
	if (!borg_spell_legal(realm, book, what)) return (FALSE);

	/* Hack -- blind/confused */
	if (bp_ptr->status.blind ||
		bp_ptr->status.confused) return (FALSE);

	/* The spell must be affordable (now) */
	if (borg_spell_mana(realm, book, what) > bp_ptr->csp) return (FALSE);

	/* Check if this spell uses reserve mana */
	if (!borg_reserve_allow(realm, book, what)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * fail rate on a spell
 */
int borg_spell_fail_rate(int realm, int book, int what)
{
	int chance, minfail, stat, power;

	list_item *l_ptr;
	borg_magic *as = &borg_magics[realm][book][what];

	/* Warriors can't cast spells */
	if (borg_class == CLASS_WARRIOR) return (100);

	/* Access the spell  */
	chance = as->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (bp_ptr->lev - as->level);

	/* Is the borg an INT user? */
	if (bp_ptr->intmana)
	{
		/* Get the value of INT */
		stat = my_stat_ind[A_INT];
	}
	/* Is the borg a WIS user? */
	else if (bp_ptr->wismana)
	{
		/* Get the value of WIS */
		stat = my_stat_ind[A_WIS];
	}
	else
	{
		/* Inconcievable! */
		borg_oops("A spellcaster that doesn't need INT or WIS!");

		/* fail the spell */
		return (100);
	}

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[stat] - 1);

	/* Collect the spell cost */
	power = borg_spell_mana(realm, book, what);

	/* Failure rate goes up if there is not enough mana */
	if (power > bp_ptr->csp) chance += 5 * (power - bp_ptr->csp);
	
	/* Some mutations increase spell failure */
	if (bp_ptr->muta3 & MUT3_MAGIC_RES ||
		bp_ptr->muta1 & MUT1_EAT_MAGIC) chance += 5;

	/*
	 * What realm is that????
	 * spell_chance in spells3.c has the -1 here, but I got confused with
	 * the -1 that are in the calls to spell_chance too.  I am guessing it
	 * refers here to REALM_DEATH that already has a banish.
	 */
/*	if (realm == REALM_DEATH - 1 &&*/
	if (realm == REALM_DEATH &&
		bp_ptr->muta1 & MUT1_BANISH) chance += 10;

	/* Squeeeeeek */
	if (bp_ptr->muta3 & MUT3_SILLY_VOI) chance += as->level;

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[stat];

	/* Non mage characters never get too good */
	if (borg_class != CLASS_MAGE &&
		borg_class != CLASS_PRIEST &&
		borg_class != CLASS_HIGH_MAGE &&
		borg_class != CLASS_MINDCRAFTER)
	{
		/* For these the minfail is at least 5% */
		minfail = MAX(5, minfail);
	}

	/* If the borg is a priest with an non-blessed edged weapon */
	if (borg_class == CLASS_PRIEST)
	{
		l_ptr = &equipment[EQUIP_WIELD];

		if (l_ptr->k_idx &&
			(l_ptr->tval == TV_SWORD || l_ptr->tval == TV_POLEARM) &&
			!KN_FLAG(l_ptr, TR_BLESSED))
		{
			/* Penalize this edgy priest */
			chance += 25;
		}
	}

	/* Minimum failure rate */
	chance = MAX(chance, minfail);

	/* Stunning makes spells harder */
	if (bp_ptr->status.heavy_stun) chance += 25;
	if (bp_ptr->status.stun) chance += 15;

	/* Always a 5 percent chance of working */
	chance = MIN(chance, 95);

	/* Return the chance */
	return (chance);
}

/*
 * same as borg_spell_okay with a fail % check
 */
bool borg_spell_okay_fail(int realm, int book, int what, int allow_fail)
{
	if (borg_spell_fail_rate(realm, book, what) > allow_fail)
		return FALSE;
	return borg_spell_okay(realm, book, what);
}

/*
 * Same as borg_spell with a fail % check
 */
bool borg_spell_fail(int realm, int book, int what, int allow_fail)
{
	if (borg_spell_fail_rate(realm, book, what) > allow_fail)
		return FALSE;
	return borg_spell(realm, book, what);
}

/*
 * Same as borg_spell_legal with a fail % check
 */
bool borg_spell_legal_fail(int realm, int book, int what, int allow_fail)
{
	if (borg_spell_fail_rate(realm, book, what) > allow_fail)
		return FALSE;
	return borg_spell_legal(realm, book, what);
}

/*
 * Attempt to cast a spell
 */
bool borg_spell(int realm, int book, int what)
{
	int i;

	borg_magic *as = &borg_magics[realm][book][what];

	/* Require ability (right now) */
	if (!borg_spell_okay(realm, book, what)) return (FALSE);

	/* Not if locked down */
	if (FLAG(bp_ptr, TR_NO_MAGIC)) return (FALSE);

	/* Look for the book */
	i = borg_book[realm][book];

	/* Paranoia */
	if (i < 0) return (FALSE);

	/* Debugging Info */
	borg_note_fmt("# Casting %s (%d,%d).", as->name, book, what);

	/* Cast a spell */
	borg_keypress('m');
	borg_keypress(I2A(i));
	borg_keypress(I2A(what));

	/* increment the spell counter */
	as->times++;

	/* Dimension Door -- Must target the landing zone */
	if ((realm == REALM_SORCERY && book == 2 && what == 3) ||
		(realm == REALM_TRUMP && book == 0 && what == 5))
	{
		borg_dimension_door();
	}

	/* Success */
	return (TRUE);
}

/* Determines if a book contains spells that can be reliably cast */
bool borg_uses_book(int realm, int book)
{
	int spell;

	/* Loop through the spells */
	for (spell = 0; spell < 8; spell++)
	{
		/* Is this an easy spell? */
		if (borg_spell_legal_fail(realm, book, spell, 40)) return (TRUE);
	}

	/* Only hard / impossible spells */
	return (FALSE);
}


/*** Mindcrafter spells are much like realm spells ***/

/* Determine if the borg can cast a given spell with regard to reserve_mana */
static bool borg_reserve_allow_mindcrafter(int spell)
{
	borg_mind *as = &borg_minds[spell];

	if (bp_ptr->csp - as->power >= borg_reserve_mana()) return (TRUE);

	/* Minor Displacement spells ok */
	if (spell == MIND_MINOR_DISP) return (TRUE);

	/* Major Displacement ok */
	if (spell == MIND_MAJOR_DISP) return (TRUE);

	/* Telekinetic Wave ok */
	if (spell == MIND_TELE_WAVE) return (TRUE);

	/* others are rejected */
	return (FALSE);
}

/*
 * Determine if borg can cast a given Mindcraft spell (when fully rested)
 */
bool borg_mindcr_legal(int spell, int level)
{
	borg_mind *as = &borg_minds[spell];

	/* The borg must be able to "cast" spells this realm */
	if (borg_class != CLASS_MINDCRAFTER) return (FALSE);

	/* The spell must be "known" */
	if (bp_ptr->lev < level) return (FALSE);

	/* The spell must be affordable (when rested) */
	if (as->power > bp_ptr->msp) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * Determine if borg can cast a given spell (right now)
 */
bool borg_mindcr_okay(int spell, int level)
{
	borg_mind *as = &borg_minds[spell];

	/* Require ability (when rested) */
	if (!borg_mindcr_legal(spell, level)) return (FALSE);

	/* No spellcasting when confused */
	if (bp_ptr->status.confused) return (FALSE);

	/* The spell must be affordable (now) */
	if (as->power > bp_ptr->csp) return (FALSE);

	/* Do not cut into reserve mana (for final teleport) */
	if (!borg_reserve_allow_mindcrafter(spell)) return (FALSE);

	/* No go if there is an item with the NO_MAGIC flag */
	if (FLAG(bp_ptr, TR_NO_MAGIC)) return (FALSE);

	/* Success */
	return (TRUE);
}

/*
 * fail rate on a mindcrafter spell
 */
int borg_mindcr_fail_rate(int spell, int level)
{
	int chance, minfail;
	borg_mind *as = &borg_minds[spell];

	/* Hack - ignore parameter */
	(void)level;

	/* Access the spell  */
	chance = as->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (bp_ptr->lev - as->level);

	/* Reduce failure rate by WIS adjustment */
	chance -= 3 * (adj_mag_stat[my_stat_ind[A_WIS]] - 1);

	/* If there is not enough mana the fail rate plummets */
	if (as->power > bp_ptr->csp) chance += 5 * (as->power - bp_ptr->csp);
	
	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[my_stat_ind[A_WIS]];

	/* Minimum failure rate */
	chance = MAX(chance, minfail);

	/* Stunning makes spells harder */
	if (bp_ptr->status.heavy_stun) chance += 25;
	if (bp_ptr->status.stun) chance += 15;

	/* Always a 5 percent chance of working */
	chance = MIN(chance, 95);

	/* Return the chance */
	return (chance);
}

/*
 * same as borg_mind_okay with a fail % check
 */
bool borg_mindcr_okay_fail(int spell, int level, int allow_fail)
{
	if (borg_mindcr_fail_rate(spell, level) > allow_fail)
		return FALSE;
	return borg_mindcr_okay(spell, level);
}

/*
 * Same as borg_mind with a fail % check
 */
bool borg_mindcr_fail(int spell, int level, int allow_fail)
{
	if (borg_mindcr_fail_rate(spell, level) > allow_fail)
		return FALSE;
	return borg_mindcr(spell, level);
}

/*
 * Same as borg_mind_legal with a fail % check
 */
bool borg_mindcr_legal_fail(int spell, int level, int allow_fail)
{
	if (borg_mindcr_fail_rate(spell, level) > allow_fail)
		return FALSE;
	return borg_mindcr_legal(spell, level);
}

/*
 * Attempt to cast a mindcrafter spell
 */
bool borg_mindcr(int spell, int level)
{
	borg_mind *as = &borg_minds[spell];

	/* Require ability (right now) */
	if (!borg_mindcr_okay(spell, level)) return (FALSE);

	/* Not if locked down */
	if (FLAG(bp_ptr, TR_NO_MAGIC)) return (FALSE);

	/* Debugging Info */
	borg_note_fmt
		("# Casting %s (spell: %d, level: %d).", as->name, spell, level);

	/* Cast a spell */
	borg_keypress('m');
	borg_keypress(as->letter);

	/* increment the spell counter */
	as->times++;

	/* Dimension Door -- need a landing Zone */
	if (spell == MIND_MINOR_DISP && level >= 40) borg_dimension_door();

	/* Success */
	return (TRUE);
}

static bool borg_power_check(bool race, u32b which, bool check_fail,
						int lev_req, int cost, int use_stat, int difficulty)
{
	int i;
	int val;
	int sum = 0;
	int stat;

	/* Power is not available yet */
	if (bp_ptr->lev < lev_req) return (FALSE);

	/* Too confused */
	if (bp_ptr->status.confused) return FALSE;

	/* Don't use too much HP */
	if (bp_ptr->csp < cost)
	{
		/* Don't use the racial if it takes more then 70% of current HP */
		if (cost > bp_ptr->chp * 7 / 10) return (FALSE);

		/* How much can we spend? */
		if ((race && (which == RACE_GNOME || which == RACE_AMBERITE)) ||
			(!race && which == MUT1_VTELEPORT))
		{
			/* These are emergency powers, so take more risk */
			if (bp_ptr->chp < bp_ptr->mhp * 3 / 10)	return (FALSE);
		}
		else
		{
			/* Allow up to 50% of HP to be used */
			if (bp_ptr->chp < bp_ptr->mhp * 5 / 10) return (FALSE);
		}
	}
	/* Don't use too much SP */
	else
	{
		/* How much can we spend? */
		if ((race && (which == RACE_GNOME || which == RACE_AMBERITE)) ||
			(!race && which == MUT1_VTELEPORT))
		{
			/* These are emergency powers, so any mana usage is allowable */
		}
		else
		{
			/* Disallow if using the power spends reserve mana */
			if (bp_ptr->csp - cost < borg_reserve_mana()) return (FALSE);
		}
	}

	/* Legal check ends here */
	if (!check_fail) return (TRUE);

	/* Otherwise continue on to a fail check */

	/* Collect the correct stat */
	stat = my_stat_cur[use_stat];

	/* Convert the needed stat to the correct form */
    if (stat <= 180)
        stat /= 10;
    else
        stat += 18-180;

	/* Stun makes it more difficult */
	if (bp_ptr->status.stun)
	{
		difficulty += 10;
	}
	else
	{
		int lev_adj = (bp_ptr->lev - lev_req) / 3;
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if (val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	/* Finally get the fail % */
	difficulty =  100 - 100 * sum / difficulty / stat;

	/* Don't try if the fail rate is higher then 40% */
	if (difficulty >= 40)
		return (FALSE);

	/* Success */
	return (TRUE);
}


/*** Racial abilities are much like magic spells ***/

/*
 * Determine if borg can cast a given Racial spell (when fully rested).
 * -or-
 * with a reasonable degree of difficulty with Check_fail
 *
 * Ghoul and Amberite have a second power but that is ignored
 * The values for borg_power_check come from tables.c
 */
bool borg_racial_check(int race, bool check_fail)
{
	/* Who are you?  Who, who.  Who, who. */
	if (borg_race != race) return (FALSE);

	/* Tell me who are you */
	switch (race)
	{
		case RACE_HOBBIT:
			return borg_power_check(TRUE, race, check_fail, 15, 10, A_INT, 10);

		case RACE_GNOME:
			return borg_power_check(TRUE, race, check_fail, 5, 10, A_INT, 12);

		case RACE_DWARF:
			return borg_power_check(TRUE, race, check_fail, 5, 5, A_WIS, 12);

		case RACE_HALF_ORC:
			return borg_power_check(TRUE, race, check_fail, 3, 5, A_WIS, 8);

		case RACE_HALF_TROLL:
			return borg_power_check(TRUE, race, check_fail, 10, 12, A_WIS, 9);

		case RACE_AMBERITE:	/* not coded yet */
			return borg_power_check(TRUE, race, check_fail, 30, 50, A_INT, 50);

		case RACE_BARBARIAN:
			return borg_power_check(TRUE, race, check_fail, 8, 10, A_WIS, 9);

		case RACE_HALF_OGRE:
			return borg_power_check(TRUE, race, check_fail, 25, 35, A_INT, 15);

		case RACE_HALF_GIANT:
			return borg_power_check(TRUE, race, check_fail, 20, 10, A_STR, 12);

		case RACE_HALF_TITAN:
			return borg_power_check(TRUE, race, check_fail, 35, 20, A_STR, 12);

		case RACE_CYCLOPS:
			return borg_power_check(TRUE, race, check_fail, 20, 15, A_STR, 12);

		case RACE_YEEK:
			return borg_power_check(TRUE, race, check_fail, 15, 15, A_WIS, 10);

		case RACE_KLACKON:
			return borg_power_check(TRUE, race, check_fail, 9, 9, A_DEX, 14);

		case RACE_KOBOLD:
			return borg_power_check(TRUE, race, check_fail, 12, 8, A_DEX, 14);

		case RACE_NIBELUNG:
			return borg_power_check(TRUE, race, check_fail, 10, 5, A_WIS, 10);

		case RACE_DARK_ELF:
			return borg_power_check(TRUE, race, check_fail, 2, 2, A_INT, 9);

		case RACE_DRACONIAN:
			return borg_power_check(TRUE, race, check_fail, 15, 25, A_CON, 12);

		case RACE_MIND_FLAYER:
			return borg_power_check(TRUE, race, check_fail, 15, 12, A_INT, 14);

		case RACE_IMP:
			return borg_power_check(TRUE, race, check_fail, 9, 15, A_WIS, 15);

		case RACE_GOLEM:
			return borg_power_check(TRUE, race, check_fail, 20, 15, A_CON, 8);

		case RACE_SKELETON:
		case RACE_ZOMBIE:
			return borg_power_check(TRUE, race, check_fail, 30, 30, A_WIS, 18);

		case RACE_VAMPIRE:
			return borg_power_check(TRUE, race, check_fail, 5, 10, A_CON, 9);

		case RACE_SPECTRE:
			return borg_power_check(TRUE, race, check_fail, 4, 6, A_INT, 3);

		case RACE_SPRITE:
			return borg_power_check(TRUE, race, check_fail, 12, 12, A_INT, 15);

		case RACE_GHOUL:
			return borg_power_check(TRUE, race, check_fail, 1, 0, A_CON, 0);

		case RACE_HUMAN:
		case RACE_HALF_ELF:
		case RACE_ELF:
		case RACE_BEASTMAN:
		case RACE_HIGH_ELF:
		default: return (FALSE);
	}
}


/*
 * Attempt to cast a racial spell
 */
bool borg_racial(int race)
{
	/* Require ability (right now) */
	if (!borg_racial_check(race, TRUE)) return (FALSE);

	/* Debugging Info */
	borg_note("# Racial Power.");

	/* Cast a spell */
	borg_keypress('U');
	borg_keypress('a');

	/* Success */
	return (TRUE);
}


/*
 * Mutations and racial both use U and the racial comes first.
 * This procedure returns the number of racial powers for a race
 */
int borg_count_racial(int race)
{
	/* Which race is that? */
	switch (race)
	{
		/* Amberite & Ghoul have two racial powers */
		case RACE_GHOUL:
		case RACE_AMBERITE: return (2);

		/* These have one racial power */
		case RACE_HOBBIT:
		case RACE_GNOME:
		case RACE_DWARF:
		case RACE_HALF_ORC:
		case RACE_HALF_TROLL:
		case RACE_BARBARIAN:
		case RACE_HALF_OGRE:
		case RACE_HALF_GIANT:
		case RACE_HALF_TITAN:
		case RACE_CYCLOPS:
		case RACE_YEEK:
		case RACE_KLACKON:
		case RACE_KOBOLD:
		case RACE_NIBELUNG:
		case RACE_DARK_ELF:
		case RACE_DRACONIAN:
		case RACE_MIND_FLAYER:
		case RACE_IMP:
		case RACE_GOLEM:
		case RACE_SKELETON:
		case RACE_ZOMBIE:
		case RACE_VAMPIRE:
		case RACE_SPECTRE:
		case RACE_SPRITE: return (1);

		/* No such luck for these guys */
		case RACE_HUMAN:
		case RACE_HALF_ELF:
		case RACE_ELF:
		case RACE_BEASTMAN:
		case RACE_HIGH_ELF:
		default: return (0);
	}
}


/* Give every mutation its stats.  These numbers come from tables.c */
bool borg_mutation_check(u32b mutation, bool check)
{
	/* Is this mutation available? */
	if (!(bp_ptr->muta1 & mutation)) return (FALSE);

	switch (mutation)
	{
		case MUT1_SPIT_ACID:
			return borg_power_check(FALSE, mutation, check, 9, 9, A_DEX, 15);

		case MUT1_BR_FIRE:
			return borg_power_check(FALSE, mutation, check, 20, 20, A_CON, 18);

		case MUT1_HYPN_GAZE:
			return borg_power_check(FALSE, mutation, check, 12, 12, A_CHR, 18);

		case MUT1_TELEKINES:
			return borg_power_check(FALSE, mutation, check, 9, 9, A_WIS, 14);

		case MUT1_VTELEPORT:
			return borg_power_check(FALSE, mutation, check, 7, 7, A_WIS, 15);

		case MUT1_MIND_BLST:
			return borg_power_check(FALSE, mutation, check, 5, 3, A_WIS, 15);

		case MUT1_RADIATION:
			return borg_power_check(FALSE, mutation, check, 15, 15, A_CON, 14);

		case MUT1_VAMPIRISM:
			return borg_power_check(FALSE, mutation, check, 10, 10, A_CON, 9);

		case MUT1_SMELL_MET:
			return borg_power_check(FALSE, mutation, check, 3, 2, A_INT, 12);

		case MUT1_SMELL_MON:
			return borg_power_check(FALSE, mutation, check, 5, 4, A_INT, 15);

		case MUT1_BLINK:
			return borg_power_check(FALSE, mutation, check, 3, 3, A_WIS, 12);

		case MUT1_EAT_ROCK:
			return borg_power_check(FALSE, mutation, check, 8, 12, A_CON, 18);

		case MUT1_SWAP_POS:
			return borg_power_check(FALSE, mutation, check, 15, 12, A_DEX, 16);

		case MUT1_SHRIEK:
			return borg_power_check(FALSE, mutation, check, 20, 14, A_CON, 16);

		case MUT1_ILLUMINE:
			return borg_power_check(FALSE, mutation, check, 3, 2, A_INT, 10);

		case MUT1_DET_CURSE:
			return borg_power_check(FALSE, mutation, check, 7, 14, A_WIS, 14);

		case MUT1_BERSERK:
			return borg_power_check(FALSE, mutation, check, 8, 8, A_STR, 14);

		case MUT1_POLYMORPH:
			return borg_power_check(FALSE, mutation, check, 18, 20, A_CON, 18);

		case MUT1_MIDAS_TCH:
			return borg_power_check(FALSE, mutation, check, 10, 5, A_INT, 12);

		case MUT1_GROW_MOLD:
			return borg_power_check(FALSE, mutation, check, 1, 6, A_CON, 14);

		case MUT1_RESIST:
			return borg_power_check(FALSE, mutation, check, 10, 12, A_CON, 12);

		case MUT1_EARTHQUAKE:
			return borg_power_check(FALSE, mutation, check, 12, 12, A_STR, 16);

		case MUT1_EAT_MAGIC:
			return borg_power_check(FALSE, mutation, check, 17, 1, A_WIS, 15);

		case MUT1_WEIGH_MAG:
			return borg_power_check(FALSE, mutation, check, 6, 6, A_INT, 10);

		case MUT1_STERILITY:
			return borg_power_check(FALSE, mutation, check, 12, 23, A_CHR, 15);

		case MUT1_PANIC_HIT:
			return borg_power_check(FALSE, mutation, check, 10, 12, A_DEX, 14);

		case MUT1_DAZZLE:
			return borg_power_check(FALSE, mutation, check, 7, 15, A_CHR, 8);

		case MUT1_LASER_EYE:
			return borg_power_check(FALSE, mutation, check, 7, 10, A_WIS, 9);

		case MUT1_RECALL:
			return borg_power_check(FALSE, mutation, check, 17, 50, A_INT, 16);

		case MUT1_BANISH:
			return borg_power_check(FALSE, mutation, check, 25, 25, A_WIS, 18);

		case MUT1_COLD_TOUCH:
			return borg_power_check(FALSE, mutation, check, 2, 2, A_CON, 11);

		case MUT1_LAUNCHER:
			return borg_power_check(FALSE, mutation, check, 10, 15, A_STR, 6);

		default: return (FALSE);
	}
}


/*
 * Attempt to cast a mutational spell
 */
bool borg_mutation(u32b mutation)
{
	int i, spell;
	u32b mut_nr = 0;

	/* Require ability (right now) */
	if (!borg_mutation_check(mutation, TRUE)) return (FALSE);

	/* Find out if the there isn't a racial in the way */
	spell = borg_count_racial(borg_race) - 1;

	/* Loop through all the bits in bp_ptr->muta1 */
	for (i = 1; i < 32; i++)
	{
		/* get the current mutation */
		mut_nr = (mut_nr) ? mut_nr * 2 : 1;

		/* Does the borg have this mutation? */
		if (!(bp_ptr->muta1 & mut_nr)) continue;

		/* Advance the letter index */
		spell += 1;

		/* Is this the mutation? (Must have it at some point) */
		if (mut_nr == mutation) break;
	}

	/* Debugging Info */
	borg_note("# Mutated Power.");

	/* Cast a spell */
	borg_keypress('U');
	borg_keypress(I2A(spell));

	/* Success */
	return (TRUE);
}


/*
 * Hack -- Cheat the "spell" info
 *
 * Hack -- note the use of the "cheat" field for efficiency
 */
void borg_cheat_spell(int realm)
{
	int j, what;
	int book;

	/* Can we use spells/prayers? */
	if (realm == 0) return;


	/* process books */
	for (book = 0; book < 4; book++)
	{
		/* Process the spells */
		for (what = 0; what < 8; what++)
		{
			/* Access the spell */
			borg_magic *as = &borg_magics[realm][book][what];

			/* Skip illegible spells */
			if (as->status == BORG_MAGIC_ICKY) continue;

			/* Access the index */
			j = as->cheat;

			/* Note "forgotten" spells */
			if ((realm == bp_ptr->realm1) ?
				((p_ptr->spell.r[0].forgotten & (1L << j))) :
				((p_ptr->spell.r[1].forgotten & (1L << j))))
			{
				/* Forgotten */
				as->status = BORG_MAGIC_LOST;
			}

			/* Note "difficult" spells */
			else if (bp_ptr->lev < as->level)
			{
				/* Unknown */
				as->status = BORG_MAGIC_HIGH;
			}

			/* Note "unknown" spells */
			else if (!((realm == bp_ptr->realm1) ?
					   (p_ptr->spell.r[0].learned & (1L << j)) :
					   (p_ptr->spell.r[1].learned & (1L << j))))
			{
				/* Unknown */
				as->status = BORG_MAGIC_OKAY;
			}

			/* Note "untried" spells */
			else if (!((realm == bp_ptr->realm1) ?
					   (p_ptr->spell.r[0].learned & (1L << j)) :
					   (p_ptr->spell.r[1].learned & (1L << j))))
			{
				/* Untried */
				as->status = BORG_MAGIC_TEST;
			}

			/* Note "known" spells */
			else
			{
				/* Known */
				as->status = BORG_MAGIC_KNOW;
			}
		}						/* book */
	}							/* Realm */
}


/*
 * Prepare a book
 */
static void prepare_book_info(int realm, int book)
{
	int i, what;

	int spell[64], num = 0;

	/* Reset each spell entry */
	for (what = 0; what < 8; what++)
	{
		borg_magic *as = &borg_magics[realm][book][what];

		/* Assume no name */
		as->name = NULL;

		/* Know the Realm, if any */
		as->realm = realm;

		/* Assume illegible */
		as->status = BORG_MAGIC_ICKY;

		/* Assume illegible */
		as->method = BORG_MAGIC_ICK;

		/* Impossible values */
		as->level = 99;
		as->power = 99;

		/* Impossible value */
		as->cheat = 99;

		/* Delete the text name */
		as->realm_name = NULL;
	}


	/* Can we use spells/prayers? */
	if (borg_class == CLASS_WARRIOR) return;


	/* Extract spells */
	for (i = 0; i < 32; i++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[book] & (1L << i)))
		{
			/* Collect this spell */
			spell[num++] = i;
		}
	}

	/* Process each existing spell */
	for (what = 0; what < num; what++)
	{
		borg_magic *as = &borg_magics[realm][book][what];

		magic_type *s_ptr = &pmb_ptr->info[realm - 1][spell[what]];

		/* Skip "illegible" spells */
		if (s_ptr->slevel == 99) continue;

		/* Save the spell name */
		as->name = spell_names[realm - 1][spell[what]];

		/* Realm Name */
		if (realm == 1) as->realm_name = "Life";
		if (realm == 2) as->realm_name = "Sorcery";
		if (realm == 3) as->realm_name = "Nature";
		if (realm == 4) as->realm_name = "Chaos";
		if (realm == 5) as->realm_name = "Death";
		if (realm == 6) as->realm_name = "Trump";
		if (realm == 7) as->realm_name = "Arcane";

		/* Save the Realm, if any */
		as->realm = realm;

		/* Save the spell index */
		as->cheat = spell[what];

		/* Hack -- assume excessive level */
		as->status = BORG_MAGIC_HIGH;

		/* Access the correct "method" */
		as->method = borg_magic_method[realm][book][what];

		/* Access the correct "rating" */
		as->rating = borg_magic_rating[realm][book][what];

		/* Extract the level and power */
		as->level = s_ptr->slevel;
		as->power = s_ptr->smana;

		/* extract fail rate. */
		as->sfail = s_ptr->sfail;
	}
}

/*
 * Prepare a Mindcrafter Array
 */
static void prepare_mind_info(void)
{
	int spell;

	/* Reset each spell entry */
	for (spell = 0; spell < MINDCRAFT_MAX; spell++)
	{
		borg_mind *as = &borg_minds[spell];
		mindcraft_power *s_ptr = &mindcraft_powers[spell];

		/* name */
		as->name = s_ptr->name;

		/* values */
		as->level = s_ptr->min_lev;
		as->power = s_ptr->mana_cost;

		/* Fail Rate */
		as->sfail = s_ptr->fail;

		/* Delete the text letter address */
		as->letter = 'a' + spell;

	}
}


/*
 * Hack -- prepare some stuff based on the player race and class
 */
void prepare_race_class_info(void)
{
	int book;

	/* Hack -- Realms */
	bp_ptr->realm1 = p_ptr->spell.r[0].realm;
	bp_ptr->realm2 = p_ptr->spell.r[1].realm;

	/* Initialize the various spell arrays by book */
	for (book = 0; book < 4; book++)
	{
		prepare_book_info(bp_ptr->realm1, book);
		prepare_book_info(bp_ptr->realm2, book);
	}

	/* MindCrafters */
	if (borg_class == CLASS_MINDCRAFTER)
	{
		prepare_mind_info();
	}
}

/*
 * Initialize this file
 */
void borg_init_3(void)
{
	/* Track the shop locations */
	track_shop_num = 0;
	track_shop_size = 16;

	/* Make the stores in the town */
	C_MAKE(borg_shops, track_shop_size, borg_shop);
}

#else

#ifdef MACINTOSH
static int HACK = 0;
#endif

#endif
