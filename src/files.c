/* File: files.c */

/* Purpose: code dealing with files (and death) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

#ifdef SAFE_SETUID

#ifdef HAVE_SETEGID

	if (setegid(getgid()) != 0)
	{
		quit("setegid(): cannot set permissions correctly!");
	}

#else  /* HAVE_SETEGID */

#ifdef SAFE_SETUID_POSIX

	if (setgid(getgid()) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

#else  /* SAFE_SETUID_POSIX */

	if (setregid(getegid(), getgid()) != 0)
	{
		quit("setregid(): cannot set permissions correctly!");
	}

#endif /* SAFE_SETUID_POSIX */

#endif /* HAVE_SETEGID */

#endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Hack -- grab permissions
 */
void safe_setuid_grab(void)
{

#ifdef SET_UID

#ifdef SAFE_SETUID

#ifdef HAVE_SETEGID

	if (setegid(player_egid) != 0)
	{
		quit("setegid(): cannot set permissions correctly!");
	}

#else  /* HAVE_SETEGID */

#ifdef SAFE_SETUID_POSIX

	if (setgid(player_egid) != 0)
	{
		quit("setgid(): cannot set permissions correctly!");
	}

#else  /* SAFE_SETUID_POSIX */

	if (setregid(getegid(), getgid()) != 0)
	{
		quit("setregid(): cannot set permissions correctly!");
	}

#endif /* SAFE_SETUID_POSIX */

#endif /* HAVE_SETEGID */

#endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 *
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 *
 * Hack -- We will always extract at least one token
 */
s16b tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;

	char *s = buf;


	/* Process */
	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if ((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			if ((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				/* Advance */
				t++;

				/* Handle backslash */
				if (*t == '\\') t++;

				/* Require a character */
				if (!*t) break;

				/* Advance */
				t++;

				/* Hack -- Require a close quote */
				if (*t != '\'') *t = '\'';
			}

			/* Handle back-slash */
			if (*t == '\\') t++;
		}

		/* Nothing left */
		if (!*t) break;

		/* Nuke and advance */
		*t++ = '\0';

		/* Save the token */
		tokens[i++] = s;

		/* Advance */
		s = t;
	}

	/* Save the token */
	tokens[i++] = s;

	/* Number found */
	return (i);
}


/* A number with a name */
typedef struct named_num named_num;

struct named_num
{
	cptr name;	/* The name of this thing */
	int num;	/* A number associated with it */
};


/* Index of spell type names */
static const named_num gf_desc[] =
{
	{"GF_ELEC", GF_ELEC},
	{"GF_POIS", GF_POIS},
	{"GF_ACID", GF_ACID},
	{"GF_COLD", GF_COLD},
	{"GF_FIRE", GF_FIRE},
	{"GF_MISSILE", GF_MISSILE},
	{"GF_ARROW", GF_ARROW},
	{"GF_PLASMA", GF_PLASMA},
	{"GF_WATER", GF_WATER},
	{"GF_LITE", GF_LITE},
	{"GF_DARK", GF_DARK},
	{"GF_LITE_WEAK", GF_LITE_WEAK},
	{"GF_DARK_WEAK", GF_DARK_WEAK},
	{"GF_SHARDS", GF_SHARDS},
	{"GF_SOUND", GF_SOUND},
	{"GF_CONFUSION", GF_CONFUSION},
	{"GF_FORCE", GF_FORCE},
	{"GF_INERTIA", GF_INERTIA},
	{"GF_MANA", GF_MANA},
	{"GF_METEOR", GF_METEOR},
	{"GF_ICE", GF_ICE},
	{"GF_CHAOS", GF_CHAOS},
	{"GF_NETHER", GF_NETHER},
	{"GF_DISENCHANT", GF_DISENCHANT},
	{"GF_NEXUS", GF_NEXUS},
	{"GF_TIME", GF_TIME},
	{"GF_GRAVITY", GF_GRAVITY},
	{"GF_KILL_WALL", GF_KILL_WALL},
	{"GF_KILL_DOOR", GF_KILL_DOOR},
	{"GF_KILL_TRAP", GF_KILL_TRAP},
	{"GF_MAKE_WALL", GF_MAKE_WALL},
	{"GF_MAKE_DOOR", GF_MAKE_DOOR},
	{"GF_MAKE_TRAP", GF_MAKE_TRAP},
	{"GF_OLD_CLONE", GF_OLD_CLONE},
	{"GF_OLD_POLY", GF_OLD_POLY},
	{"GF_OLD_HEAL", GF_OLD_HEAL},
	{"GF_OLD_SPEED", GF_OLD_SPEED},
	{"GF_OLD_SLOW", GF_OLD_SLOW},
	{"GF_OLD_CONF", GF_OLD_CONF},
	{"GF_OLD_SLEEP", GF_OLD_SLEEP},
	{"GF_OLD_DRAIN", GF_OLD_DRAIN},
	{"GF_NEW_DRAIN", GF_NEW_DRAIN},
	{"GF_AWAY_UNDEAD", GF_AWAY_UNDEAD},
	{"GF_AWAY_EVIL", GF_AWAY_EVIL},
	{"GF_AWAY_ALL", GF_AWAY_ALL},
	{"GF_TURN_UNDEAD", GF_TURN_UNDEAD},
	{"GF_TURN_EVIL", GF_TURN_EVIL},
	{"GF_TURN_ALL", GF_TURN_ALL},
	{"GF_DISP_UNDEAD", GF_DISP_UNDEAD},
	{"GF_DISP_EVIL", GF_DISP_EVIL},
	{"GF_DISP_ALL", GF_DISP_ALL},
	{"GF_DISP_DEMON", GF_DISP_DEMON},
	{"GF_DISP_LIVING", GF_DISP_LIVING},
	{"GF_ROCKET", GF_ROCKET},
	{"GF_NUKE", GF_NUKE},
	{"GF_MAKE_GLYPH", GF_MAKE_GLYPH},
	{"GF_STASIS", GF_STASIS},
	{"GF_STONE_WALL", GF_STONE_WALL},
	{"GF_DEATH_RAY", GF_DEATH_RAY},
	{"GF_STUN", GF_STUN},
	{"GF_HOLY_FIRE", GF_HOLY_FIRE},
	{"GF_HELL_FIRE", GF_HELL_FIRE},
	{"GF_DISINTEGRATE", GF_DISINTEGRATE},
	{"GF_CHARM", GF_CHARM},
	{"GF_CONTROL_UNDEAD", GF_CONTROL_UNDEAD},
	{"GF_CONTROL_ANIMAL", GF_CONTROL_ANIMAL},
	{"GF_PSI", GF_PSI},
	{"GF_PSI_DRAIN", GF_PSI_DRAIN},
	{"GF_TELEKINESIS", GF_TELEKINESIS},
	{"GF_JAM_DOOR", GF_JAM_DOOR},
	{"GF_DOMINATION", GF_DOMINATION},
	{"GF_DISP_GOOD", GF_DISP_GOOD},
	{NULL, 0}
};


/*
 * Parse a sub-file of the "extra info" (format shown below)
 *
 * Each "action" line has an "action symbol" in the first column,
 * followed by a colon, followed by some command specific info,
 * usually in the form of "tokens" separated by colons or slashes.
 *
 * Blank lines, lines starting with white space, and lines starting
 * with pound signs ("#") are ignored (as comments).
 *
 * Note the use of "tokenize()" to allow the use of both colons and
 * slashes as delimeters, while still allowing final tokens which
 * may contain any characters including "delimiters".
 *
 * Note the use of "strtol()" to allow all "integers" to be encoded
 * in decimal, hexidecimal, or octal form.
 *
 * Note that "monster zero" is used for the "player" attr/char, "object
 * zero" will be used for the "stack" attr/char, and "feature zero" is
 * used for the "nothing" attr/char.
 *
 * Parse another file recursively, see below for details.
 *   %:<filename>
 *
 * Specify the attr/char values for "monsters" by race index.
 *   R:<num>:<a>:<c>
 *
 * Specify the attr/char values for "objects" by kind index.
 *   K:<num>:<a>:<c>
 *
 * Specify the attr/char values for "features" by feature index.
 *   F:<num>:<a>:<c>
 *
 * Specify the attr/char values for "fields" by field index.
 *   T:<num>:<a>:<c>
 *
 * Specify the attr/char values for unaware "objects" by kind tval.
 *   U:<tv>:<a>:<c>
 *
 * Specify the attr/char values for inventory "objects" by kind tval.
 *   E:<tv>:<a>:<c>
 *
 * Define a macro action, given an encoded macro action.
 *   A:<str>
 *
 * Create a normal macro, given an encoded macro trigger.
 *   P:<str>
 *
 * Create a command macro, given an encoded macro trigger.
 *   C:<str>
 *
 * Create a keyset mapping.
 *   S:<key>:<key>:<dir>
 *
 * Turn an option off, given its name.
 *   X:<str>
 *
 * Turn an option on, given its name.
 *   Y:<str>
 *
 * Specify visual information, given an index, and some data.
 *   V:<num>:<kv>:<rv>:<gv>:<bv>
 *
 * Specify colors for message-types.
 *   M:<type>:<attr>
 *
 * Specify the set of colors to use when drawing a zapped spell.
 *   Z:<type>:<str>
 */
errr process_pref_file_command(char *buf)
{
	int i, j, n1, n2;

	char *zz[16];


	/* Skip "empty" lines */
	if (!buf[0]) return (0);

	/* Skip "blank" lines */
	if (isspace(buf[0])) return (0);

	/* Skip comments */
	if (buf[0] == '#') return (0);

	/* Require "?:*" format */
	if (buf[1] != ':') return (1);


	/* Process "%:<fname>" */
	if (buf[0] == '%')
	{
		/* Attempt to Process the given file */
		return (process_pref_file("%s", buf + 2));
	}


	/* Process "R:<num>:<a>/<c>" -- attr/char for monster races */
	if (buf[0] == 'R')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			monster_race *r_ptr;
			i = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((i < 0) || (i >= z_info->r_max)) return (1);
			r_ptr = &r_info[i];
			if (n1) r_ptr->x_attr = n1;
			if (n2) r_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	else if (buf[0] == 'K')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *k_ptr;
			i = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((i < 0) || (i >= z_info->k_max)) return (1);
			k_ptr = &k_info[i];
			if (n1) k_ptr->x_attr = n1;
			if (n2) k_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "T:<num>:<a>/<c>" -- attr/char for fields */
	else if (buf[0] == 'T')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			field_thaum *t_ptr;
			i = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((i < 0) || (i >= z_info->t_max)) return (1);
			t_ptr = &t_info[i];
			if (n1) t_ptr->f_attr = n1;
			if (n2) t_ptr->f_char = n2;
			return (0);
		}
	}

	/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
	else if (buf[0] == 'F')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			feature_type *f_ptr;
			i = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((i < 0) || (i >= z_info->f_max)) return (1);
			f_ptr = &f_info[i];
			if (n1) f_ptr->x_attr = n1;
			if (n2) f_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "W:<num>:<a>/<c>" -- xtra attr/char for terrain features */
	else if (buf[0] == 'W')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			feature_type *f_ptr;
			i = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((i < 0) || (i >= z_info->f_max)) return (1);
			f_ptr = &f_info[i];
			if (n1) f_ptr->w_attr = n1;
			if (n2) f_ptr->w_char = n2;
			return (0);
		}
	}

	/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	else if (buf[0] == 'S')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if ((j < 0) || (j >= 256)) return (1);
			misc_to_attr[j] = n1;
			misc_to_char[j] = n2;
			return (0);
		}
	}

	/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	else if (buf[0] == 'U')
	{
		if (tokenize(buf + 2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge) strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			for (i = 1; i < z_info->k_max; i++)
			{
				object_kind *k_ptr = &k_info[i];
				if (k_ptr->tval == j)
				{
					if (n1) k_ptr->d_attr = n1;
					if (n2) k_ptr->d_char = n2;
				}
			}
			return (0);
		}
	}

	/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	else if (buf[0] == 'E')
	{
		if (tokenize(buf + 2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = strtol(zz[1], NULL, 0);
			if ((j < 0) || (j >= 128)) return (1);
			if (n1) tval_to_attr[j] = n1;
			return (0);
		}
	}


	/* Process "A:<str>" -- save an "action" for later */
	else if (buf[0] == 'A')
	{
		text_to_ascii(macro__buf, buf + 2);
		return (0);
	}

	/* Process "P:<str>" -- normal macro */
	else if (buf[0] == 'P')
	{
		char tmp[1024];
		text_to_ascii(tmp, buf + 2);
		macro_add(tmp, macro__buf);
		return (0);
	}


	/* Process "C:<str>" -- create keymap */
	else if (buf[0] == 'C')
	{
		int mode;

		char tmp[1024];

		if (tokenize(buf + 2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return (1);

		mode = strtol(zz[0], NULL, 0);
		if ((mode < 0) || (mode >= KEYMAP_MODES)) return (1);

		text_to_ascii(tmp, zz[1]);
		if (!tmp[0] || tmp[1]) return (1);
		i = (byte)(tmp[0]);

		string_free(keymap_act[mode][i]);

		keymap_act[mode][i] = string_make(macro__buf);

		return (0);
	}


	/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	else if (buf[0] == 'V')
	{
		if (tokenize(buf + 2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			if ((i < 0) || (i >= 256)) return (1);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return (0);
		}
	}


	/* Process "X:<str>" -- turn option off */
	else if (buf[0] == 'X')
	{
		for (i = 0; i < OPT_MAX; i++)
		{
			if (option_info[i].o_desc &&
				(option_info[i].o_page != OPT_BIRTH_PAGE) &&
				option_info[i].o_text && streq(option_info[i].o_text, buf + 2))
			{
				/* Clear */
				option_info[i].o_val = FALSE;

				/* Save the change */
				if (character_generated)
				{
					init_options(OPT_FLAG_SERVER | OPT_FLAG_PLAYER);
				}
				else
				{
					init_options(OPT_FLAG_BIRTH | OPT_FLAG_SERVER |
								 OPT_FLAG_PLAYER);
				}

				return (0);
			}
		}
		/* XXX XXX XXX - ignore unknown or birth options */
		return (0);
	}

	/* Process "Y:<str>" -- turn option on */
	else if (buf[0] == 'Y')
	{
		for (i = 0; i < OPT_MAX; i++)
		{
			if (option_info[i].o_desc &&
				(option_info[i].o_page != OPT_BIRTH_PAGE) &&
				option_info[i].o_text && streq(option_info[i].o_text, buf + 2))
			{
				/* Set */
				option_info[i].o_val = TRUE;

				/* Save the change */
				if (character_generated)
				{
					init_options(OPT_FLAG_SERVER | OPT_FLAG_PLAYER);
				}
				else
				{
					init_options(OPT_FLAG_BIRTH | OPT_FLAG_SERVER |
								 OPT_FLAG_PLAYER);
				}

				return (0);
			}
		}

		/* XXX XXX XXX - ignore unknown or birth options */
		return (0);
	}

	/* Process "Z:<type>:<str>" -- set spell color */
	else if (buf[0] == 'Z')
	{
		/* Find the colon */
		char *t = strchr(buf + 2, ':');

		/* Oops */
		if (!t) return (1);

		/* Nuke the colon */
		*(t++) = '\0';

		for (i = 0; gf_desc[i].name; i++)
		{
			/* Match this type */
			if (streq(gf_desc[i].name, buf + 2))
			{
				/* Wipe old strings */
				if (gf_color[gf_desc[i].num])
				{
					string_free(gf_color[gf_desc[i].num]);
				}

				/* Remember this color set */
				gf_color[gf_desc[i].num] = string_make(t);

				/* Success */
				return (0);
			}
		}
	}

	/* Process "M:<type>:<attr>" -- colors for message-types */
	else if (buf[0] == 'M')
	{
		if (tokenize(buf + 2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			u16b type = (u16b)strtol(zz[0], NULL, 0);
			int color = color_char_to_attr(zz[1][0]);

			/* Ignore illegal color */
			if (color < 0) return (1);

			/* Store the color */
			return (message_color_define(type, (byte)color));
		}
	}


	/* Failure */
	return (1);
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
static cptr process_pref_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	while (isspace(*s)) s++;

	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_pref_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(p, t)) v = "0";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) >= 0)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) <= 0)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		if (f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
		while (isprint(*s) && !strchr(" []", *s)) ++s;

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if (*b == '$')
		{
			/* System */
			if (streq(b + 1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			/* Graphics */
			else if (streq(b + 1, "GRAF"))
			{
				switch (use_graphics)
				{
					case GRAPHICS_NONE: v = "none";
						break;
					case GRAPHICS_ORIGINAL: v = "old";
						break;
					case GRAPHICS_ADAM_BOLT: v = "new";
						break;
					case GRAPHICS_ANY: v = "error";
						break;
					case GRAPHICS_HALF_3D: v = "none";
						break;
				}
			}

			/* Monochrome mode */
			else if (streq(b + 1, "MONOCHROME"))
			{
				if (arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			/* Race */
			else if (streq(b + 1, "RACE"))
			{
				v = rp_ptr->title;
			}

			/* Class */
			else if (streq(b + 1, "CLASS"))
			{
				v = cp_ptr->title;
			}

			/* Player */
			else if (streq(b + 1, "PLAYER"))
			{
				v = player_base;
			}

			/* Town */
			else if (streq(b + 1, "TOWN"))
			{
				v = vanilla_town ? "VANILLA" : "WILDERNESS";
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}


/*
 * Open the "user pref file" and parse it.
 */
static errr process_pref_file_aux(cptr name)
{
	FILE *fp;

	char buf[1024];

	char old[1024];

	int line = -1;

	errr err = 0;

	bool bypass = FALSE;


	/* Open the file */
	fp = my_fopen(name, "r");

	/* No such file */
	if (!fp) return (-1);


	/* Process the file */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Count lines */
		line++;


		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
		if (isspace(buf[0])) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if (bypass) continue;


		/* Process "%:<file>" */
		if (buf[0] == '%')
		{
			/* Process that file if allowed */
			(void)process_pref_file("%s", buf + 2);

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* Oops */
		if (err) break;
	}


	/* Error */
	if (err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
		msgf("Error %d in line %d of file '%s'.", err, line, name);
		msgf("Parsing '%s'", old);
		message_flush();
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
errr process_pref_file(cptr fmt, ...)
{
	char buf[1024], name[1024];

	errr err = 0;
	
	va_list vp;

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(name, 1024, fmt, &vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_PREF, name);

	/* Process the pref file */
	err = process_pref_file_aux(buf);

	/* Stop at parser errors, but not at non-existing file */
	if (err < 1)
	{
		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_USER, name);

		/* Process the pref file */
		err = process_pref_file_aux(buf);
	}

	/* Result */
	return (err);
}




#ifdef CHECK_TIME

/*
 * Operating hours for ANGBAND (defaults to non-work hours)
 */
static char days[7][29] =
{
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

/*
 * Restict usage (defaults to no restrictions)
 */
static bool check_time_flag = FALSE;

#endif


/*
 * Handle CHECK_TIME
 */
errr check_time(void)
{

#ifdef CHECK_TIME

	time_t c;
	struct tm *tp;

	/* No restrictions */
	if (!check_time_flag) return (0);

	/* Check for time violation */
	c = time((time_t *) 0);
	tp = localtime(&c);

	/* Violation */
	if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') return (1);

#endif

	/* Success */
	return (0);
}



/*
 * Initialize CHECK_TIME
 */
errr check_time_init(void)
{

#ifdef CHECK_TIME

	FILE *fp;

	char buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "time.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Assume restrictions */
	check_time_flag = TRUE;

	/* Parse the file */
	while (0 == my_fgets(fp, buf, 80))
	{
		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Chop the buffer */
		buf[29] = '\0';

		/* Extract the info */
		if (prefix(buf, "SUN:")) strcpy(days[0], buf);
		if (prefix(buf, "MON:")) strcpy(days[1], buf);
		if (prefix(buf, "TUE:")) strcpy(days[2], buf);
		if (prefix(buf, "WED:")) strcpy(days[3], buf);
		if (prefix(buf, "THU:")) strcpy(days[4], buf);
		if (prefix(buf, "FRI:")) strcpy(days[5], buf);
		if (prefix(buf, "SAT:")) strcpy(days[6], buf);
	}

	/* Close it */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}



#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int cp_time[4];
	int dk_xfer[4];
	unsigned int v_pgpgin;
	unsigned int v_pgpgout;
	unsigned int v_pswpin;
	unsigned int v_pswpout;
	unsigned int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_opackets;
	int if_oerrors;
	int if_collisions;
	unsigned int v_swtch;
	long avenrun[3];
	struct timeval boottime;
	struct timeval curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime st;

	/* Success if not checking */
	if (!check_load_value) return (0);

	/* Check the load */
	if (0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if (val1 >= val2) return (1);
	}

#endif

	/* Success */
	return (0);
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE *fp;

	char buf[1024];

	char temphost[MAXHOSTNAMELEN + 1];
	char thishost[MAXHOSTNAMELEN + 1];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, 1024))
	{
		int value;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if (sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if (!streq(temphost, thishost) &&
			!streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		/* Done */
		break;
	}

	/* Close the file */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}


/*
 * Print number with header at given row, column
 */
static void prt_num(int col, int row, cptr header, long num, int wid)
{
	put_fstr(col, row, "%s   %*ld", header, wid, num);
}


/*
 * Returns a "rating" of x depending on y
 *
 * Where x is the first parameter in the list,
 * and y is the second.
 */
void likert(char *buf, uint max, cptr fmt, va_list *vp)
{
	cptr desc;
	
    int x, y;
	
	/* Unused parameter */
	(void)fmt;
	
	/* Get the first argument */
	x = va_arg(*vp, int);
	
	/* Get the second argument */
	y = va_arg(*vp, int);
	
	/* Paranoia */
	if (y <= 0) y = 1;

	/* Negative value */
	if (x < 0)
	{
		desc = CLR_L_DARK "Very Bad";
	}
	else
	{
		/* Analyze the value */
		switch (x / y)
		{
			case 0:
			case 1:
			{
				desc = CLR_RED "Bad";
				break;
			}
			case 2:
			{
				desc = CLR_L_RED "Poor";
				break;
			}
			case 3:
			case 4:
			{
				desc = CLR_ORANGE "Fair";
				break;
			}
			case 5:
			{
				desc = CLR_YELLOW "Good";
				break;
			}
			case 6:
			{
				desc = CLR_YELLOW "Very Good";
				break;
			}
			case 7:
			case 8:
			{
				desc = CLR_YELLOW "Excellent";
				break;
			}
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			{
				desc = CLR_GREEN "Superb";
				break;
			}
			case 14:
			case 15:
			case 16:
			case 17:
			{
				desc = CLR_BLUE "Chaos Rank";
				break;
			}
			default:
			{
				strnfmt(buf, max, CLR_VIOLET "Amber [%d]", (int)((((x / y) - 17) * 5) / 2));
				return;
			}
		}
	}

	/* Copy into the buffer */
	strncpy(buf, desc, max - 1);
}


/* Monk average attack damage - only used here, so not in tables.c */
static int monk_avg_damage[PY_MAX_LEVEL + 1] =
{
	0,
	250, 275, 299, 299, 306, 309, 321, 325, 328, 332,
	347, 353, 375, 450, 463, 507, 523, 537, 551, 575,
	680, 704, 723, 738, 768, 792, 812, 925, 1008, 1032,
	1061, 1074, 1160, 1178, 1303, 1326, 1400, 1435, 1476, 1500,
	1669, 1809, 1836, 1875, 2155, 2190, 2227, 2587, 2769, 2811
};

#define COL_SKILLS1		0
#define COL_SKILLS2		29
#define COL_SKILLS3		58


/*
 * Prints ratings on certain abilities
 *
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
static void display_player_abilities(void)
{
	int tmp, damdice, damsides, dambonus, blows;
	int xthn, xthb;
	int muta_att = 0;
	long avgdam;
	int energy_fire;
	int shots, shot_frac;

	object_type *o_ptr;

	if (p_ptr->muta2 & MUT2_HORNS) muta_att++;
	if (p_ptr->muta2 & MUT2_SCOR_TAIL) muta_att++;
	if (p_ptr->muta2 & MUT2_BEAK) muta_att++;
	if (p_ptr->muta2 & MUT2_TRUNK) muta_att++;
	if (p_ptr->muta2 & MUT2_TENTACLES) muta_att++;

	/* Fighting Skill (with current weapon) */
	o_ptr = &p_ptr->equipment[EQUIP_WIELD];
	tmp = p_ptr->to_h + o_ptr->to_h;
	xthn = p_ptr->skills[SKILL_THN] + (tmp * BTH_PLUS_ADJ);

	/* Shooting Skill (with current bow and normal missile) */
	o_ptr = &p_ptr->equipment[EQUIP_BOW];
	tmp = p_ptr->to_h + o_ptr->to_h;
	xthb = p_ptr->skills[SKILL_THB] + (tmp * BTH_PLUS_ADJ);

	/* Is the player is wielding a shooter? */
	if (o_ptr->k_idx)
	{
		energy_fire = p_ptr->bow_energy;
	}
	else
	{
		energy_fire = 100;
	}

	/* Calculate shots per round  - note "strange" formula. */

	/* The real number of shots per round is (1 + n)/2 */
	shots = (1 + p_ptr->num_fire) * 50;
	shot_frac = (shots * 100 / energy_fire) % 100;
	shots = shots / energy_fire;

	/* Average damage per round */
	o_ptr = &p_ptr->equipment[EQUIP_WIELD];
	dambonus = p_ptr->dis_to_d;
	if (object_known_p(o_ptr)) dambonus += o_ptr->to_d;
	damdice = o_ptr->dd;
	damsides = o_ptr->ds;
	blows = p_ptr->num_blow;


	/* Basic abilities */
	put_fstr(COL_SKILLS1, 16, CLR_WHITE "Fighting    : %v\n"
							  CLR_WHITE "Bows/Throw  : %v\n"
							  CLR_WHITE "Saving Throw: %v\n"
							  CLR_WHITE "Stealth     : %v",
		 					likert, xthn, 10,
		 					likert, xthb, 10,
		 					likert, p_ptr->skills[SKILL_SAV], 10,
		 					likert, p_ptr->skills[SKILL_STL], 1);


	put_fstr(COL_SKILLS2, 16, CLR_WHITE "Perception  : %v\n"
							  CLR_WHITE "Sensing     : %v\n"
							  CLR_WHITE "Disarming   : %v\n"
							  CLR_WHITE "Magic Device: %v",
							likert, p_ptr->skills[SKILL_FOS], 6,
							likert, p_ptr->skills[SKILL_SNS], 6,
							likert, p_ptr->skills[SKILL_DIS], 8,
							likert, p_ptr->skills[SKILL_DEV], 6);

	if (!muta_att)
		put_fstr(COL_SKILLS3, 16, "Blows/Round : %d", p_ptr->num_blow);
	else
		put_fstr(COL_SKILLS3, 16, "Blows/Round : %d+%d",
					p_ptr->num_blow, muta_att);

	
	put_fstr(COL_SKILLS3, 17, "Shots/Round : %d.%d", shots, shot_frac);

	/* Effect of damage dice x2 */
	avgdam = avg_dam(dambonus, damdice, damsides);

	/* number of blows */
	avgdam *= blows;

	/* Rescale */
	avgdam /= 200;

	/* See if have a weapon with extra power */
	if (o_ptr->k_idx)
	{
		/* Is there a vorpal effect we know about? */
		if (object_known_p(o_ptr) &&
			(o_ptr->a_idx == ART_VORPAL_BLADE))
		{
			/* vorpal blade */
			avgdam *= 786;
			avgdam /= 500;
		}
		else if (object_known_p(o_ptr) && (FLAG(o_ptr, TR_VORPAL)))
		{
			/* vorpal flag only */
			avgdam *= 609;
			avgdam /= 500;
		}

		/* Estimate the effect of increased criticals */
		/* The average critical does 1.8345 * normal damage... */
		if (object_known_p(o_ptr) && (p_ptr->msp >= PSI_COST) &&
				(FLAG(o_ptr, TR_PSI_CRIT)))
		{
			avgdam *= 640;
			avgdam /= 500;
		}
	}

	/* normal players get two 1d1 punches */
	if (!o_ptr->k_idx && (p_ptr->rp.pclass != CLASS_MONK)) avgdam = 2;

	if (avgdam == 0)
	{
		if ((p_ptr->rp.pclass == CLASS_MONK) && (!o_ptr->k_idx))
			put_fstr(COL_SKILLS3, 18, "Avg.Dam./Rnd: %d",
					 monk_avg_damage[p_ptr->lev] * blows / 100);
		else
			put_fstr(COL_SKILLS3, 18, "Avg.Dam./Rnd: nil!");
	}
	else
	{
		put_fstr(COL_SKILLS3, 18, "Avg.Dam./Rnd: %d", (int)avgdam);
	}

	put_fstr(COL_SKILLS3, 19, "Infra-Vision: %d'", p_ptr->see_infra * 10);
}



/*
 * Obtain the "flags" for the player as if he was an item
 */
void player_flags(object_flags *of_ptr)
{
	/* Clear */
	of_ptr->flags[0] = 0L;
	of_ptr->flags[1] = 0L;
	of_ptr->flags[2] = 0L;
	of_ptr->flags[3] = 0L;

	/* Classes */
	switch (p_ptr->rp.pclass)
	{
		case CLASS_WARRIOR:
			if (p_ptr->lev > 29)
				SET_FLAG(of_ptr, TR_RES_FEAR);
			break;
		case CLASS_PALADIN:
			if (p_ptr->lev > 39)
				SET_FLAG(of_ptr, TR_RES_FEAR);
			break;
		case CLASS_CHAOS_WARRIOR:
			SET_FLAG(of_ptr, TR_PATRON);
			if (p_ptr->lev > 29)
				SET_FLAG(of_ptr, TR_RES_CHAOS);
			if (p_ptr->lev > 39)
				SET_FLAG(of_ptr, TR_RES_FEAR);
			break;
		case CLASS_MONK:
			/* Monks get extra abilities if unencumbered */
			if (!p_ptr->state.monk_armour_stat)
			{
				if (p_ptr->lev > 9)
					SET_FLAG(of_ptr, TR_SPEED);
				if (p_ptr->lev > 24)
					SET_FLAG(of_ptr, TR_FREE_ACT);
			}
			break;
		case CLASS_MINDCRAFTER:
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_RES_FEAR);
			if (p_ptr->lev > 19)
				SET_FLAG(of_ptr, TR_SUST_WIS);
			if (p_ptr->lev > 29)
				SET_FLAG(of_ptr, TR_RES_CONF);
			if (p_ptr->lev > 39)
				SET_FLAG(of_ptr, TR_TELEPATHY);
			break;
		default:
			;					/* Do nothing */
	}

	/* Races */
	switch (p_ptr->rp.prace)
	{
		case RACE_ELF:
			SET_FLAG(of_ptr, TR_RES_LITE);
			break;
		case RACE_HOBBIT:
			SET_FLAG(of_ptr, TR_SUST_DEX);
			break;
		case RACE_GNOME:
			SET_FLAG(of_ptr, TR_FREE_ACT);
			break;
		case RACE_DWARF:
			SET_FLAG(of_ptr, TR_RES_BLIND);
			break;
		case RACE_HALF_ORC:
			SET_FLAG(of_ptr, TR_RES_DARK);
			break;
		case RACE_HALF_TROLL:
			SET_FLAG(of_ptr, TR_SUST_STR);
			if (p_ptr->lev > 14)
			{
				SET_FLAG(of_ptr, TR_REGEN);
				if (p_ptr->rp.pclass == CLASS_WARRIOR)
				{
					SET_FLAG(of_ptr, TR_SLOW_DIGEST);
					/*
					 * Let's not make Regeneration a disadvantage
					 * for the poor warriors who can never learn
					 * a spell that satisfies hunger (actually
					 * neither can rogues, but half-trolls are not
					 * supposed to play rogues)
					 */
				}
			}
			break;
		case RACE_AMBERITE:
			SET_FLAG(of_ptr, TR_SUST_CON);
			SET_FLAG(of_ptr, TR_REGEN)	/* Amberites heal fast */;
			break;
		case RACE_HIGH_ELF:
			SET_FLAG(of_ptr, TR_RES_LITE);
			SET_FLAG(of_ptr, TR_SEE_INVIS);
			break;
		case RACE_BARBARIAN:
			SET_FLAG(of_ptr, TR_RES_FEAR);
			break;
		case RACE_HALF_OGRE:
			SET_FLAG(of_ptr, TR_SUST_STR);
			SET_FLAG(of_ptr, TR_RES_DARK);
			break;
		case RACE_HALF_GIANT:
			SET_FLAG(of_ptr, TR_RES_SHARDS);
			SET_FLAG(of_ptr, TR_SUST_STR);
			break;
		case RACE_HALF_TITAN:
			SET_FLAG(of_ptr, TR_RES_CHAOS);
			break;
		case RACE_CYCLOPS:
			SET_FLAG(of_ptr, TR_RES_SOUND);
			break;
		case RACE_YEEK:
			SET_FLAG(of_ptr, TR_RES_ACID);
			if (p_ptr->lev > 19)
				SET_FLAG(of_ptr, TR_IM_ACID);
			break;
		case RACE_KLACKON:
			SET_FLAG(of_ptr, TR_RES_CONF);
			SET_FLAG(of_ptr, TR_RES_ACID);
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_SPEED);
			break;
		case RACE_KOBOLD:
			SET_FLAG(of_ptr, TR_RES_POIS);
			break;
		case RACE_NIBELUNG:
			SET_FLAG(of_ptr, TR_RES_DISEN);
			SET_FLAG(of_ptr, TR_RES_DARK);
			break;
		case RACE_DARK_ELF:
			SET_FLAG(of_ptr, TR_RES_DARK);
			if (p_ptr->lev > 19)
				SET_FLAG(of_ptr, TR_SEE_INVIS);
			break;
		case RACE_DRACONIAN:
			SET_FLAG(of_ptr, TR_FEATHER);
			if (p_ptr->lev > 4)
				SET_FLAG(of_ptr, TR_RES_FIRE);
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_RES_COLD);
			if (p_ptr->lev > 14)
				SET_FLAG(of_ptr, TR_RES_ACID);
			if (p_ptr->lev > 19)
				SET_FLAG(of_ptr, TR_RES_ELEC);
			if (p_ptr->lev > 34)
				SET_FLAG(of_ptr, TR_RES_POIS);
			break;
		case RACE_MIND_FLAYER:
			SET_FLAG(of_ptr, TR_SUST_INT);
			SET_FLAG(of_ptr, TR_SUST_WIS);
			if (p_ptr->lev > 14)
				SET_FLAG(of_ptr, TR_SEE_INVIS);
			if (p_ptr->lev > 29)
				SET_FLAG(of_ptr, TR_TELEPATHY);
			break;
		case RACE_IMP:
			SET_FLAG(of_ptr, TR_RES_FIRE);
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_SEE_INVIS);
			break;
		case RACE_GOLEM:
			SET_FLAG(of_ptr, TR_SEE_INVIS);
			SET_FLAG(of_ptr, TR_FREE_ACT);
			SET_FLAG(of_ptr, TR_IM_POIS);
			SET_FLAG(of_ptr, TR_SLOW_DIGEST);
			SET_FLAG(of_ptr, TR_CANT_EAT);
			SET_FLAG(of_ptr, TR_HURT_COLD);
			if (p_ptr->lev > 34)
				SET_FLAG(of_ptr, TR_HOLD_LIFE);
			break;
		case RACE_SKELETON:
			SET_FLAG(of_ptr, TR_SEE_INVIS);
			SET_FLAG(of_ptr, TR_RES_SHARDS);
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
			SET_FLAG(of_ptr, TR_IM_POIS);
			SET_FLAG(of_ptr, TR_CANT_EAT);
			SET_FLAG(of_ptr, TR_HURT_ACID);
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_RES_COLD);
			break;
		case RACE_ZOMBIE:
			SET_FLAG(of_ptr, TR_SEE_INVIS);
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
			SET_FLAG(of_ptr, TR_RES_NETHER);
			SET_FLAG(of_ptr, TR_IM_POIS);
			SET_FLAG(of_ptr, TR_SLOW_DIGEST);
			SET_FLAG(of_ptr, TR_CANT_EAT);
			SET_FLAG(of_ptr, TR_HURT_FIRE);
			if (p_ptr->lev > 4)
				SET_FLAG(of_ptr, TR_RES_COLD);
			break;
		case RACE_VAMPIRE:
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
			SET_FLAG(of_ptr, TR_RES_DARK);
			SET_FLAG(of_ptr, TR_RES_NETHER);
			SET_FLAG(of_ptr, TR_LITE);
			SET_FLAG(of_ptr, TR_RES_POIS);
			SET_FLAG(of_ptr, TR_RES_COLD);
			SET_FLAG(of_ptr, TR_IM_DARK);
			SET_FLAG(of_ptr, TR_HURT_LITE);
			break;
		case RACE_SPECTRE:
			SET_FLAG(of_ptr, TR_RES_COLD);
			SET_FLAG(of_ptr, TR_SEE_INVIS);
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
			SET_FLAG(of_ptr, TR_RES_NETHER);
			SET_FLAG(of_ptr, TR_IM_POIS);
			SET_FLAG(of_ptr, TR_SLOW_DIGEST);
			SET_FLAG(of_ptr, TR_CANT_EAT);
			SET_FLAG(of_ptr, TR_PASS_WALL);
			SET_FLAG(of_ptr, TR_HURT_ELEC);
			if (p_ptr->lev > 34)
				SET_FLAG(of_ptr, TR_TELEPATHY);
			break;
		case RACE_SPRITE:
			SET_FLAG(of_ptr, TR_RES_LITE);
			SET_FLAG(of_ptr, TR_FEATHER);
			if (p_ptr->lev > 9)
				SET_FLAG(of_ptr, TR_SPEED);
			break;
		case RACE_BEASTMAN:
			SET_FLAG(of_ptr, TR_RES_SOUND);
			SET_FLAG(of_ptr, TR_RES_CONF);
			SET_FLAG(of_ptr, TR_MUTATE);
			break;
		case RACE_GHOUL:
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
			SET_FLAG(of_ptr, TR_CANT_EAT);
			if (p_ptr->lev > 9) SET_FLAG(of_ptr, TR_RES_DARK);
			if (p_ptr->lev > 19) SET_FLAG(of_ptr, TR_RES_NETHER);
			SET_FLAG(of_ptr, TR_IM_POIS);
			SET_FLAG(of_ptr, TR_RES_COLD);
			SET_FLAG(of_ptr, TR_GHOUL_TOUCH);
			SET_FLAG(of_ptr, TR_HURT_FIRE);
			break;
		default:
			;					/* Do nothing */
	}

	/* Hack - chaos patron */
	if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
	{
		SET_FLAG(of_ptr, TR_PATRON);
	}

	/* Mutations */
	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
			/* HURT_LITE would be too cruel */
			SET_FLAG(of_ptr, TR_CANT_EAT);
		}

		if (p_ptr->muta1 & MUT1_POLYMORPH)
		{
			SET_FLAG(of_ptr, TR_MUTATE);
		}

		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
			of_ptr->flags[1] &= ~(TR1_RES_FEAR);
		}

		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
			SET_FLAG(of_ptr, TR_CANT_EAT);
		}
	}
	
	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BEAK)
		{
			SET_FLAG(of_ptr, TR_CANT_EAT);
		}

		if (p_ptr->muta2 & MUT2_WARNING)
		{
			of_ptr->flags[1] &= ~(TR1_RES_FEAR);
		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
			SET_FLAG(of_ptr, TR_HURT_ELEC);
		}

		if (p_ptr->muta3 & MUT3_MORONIC)
		{
			SET_FLAG(of_ptr, TR_RES_FEAR);
			SET_FLAG(of_ptr, TR_RES_CONF);
		}

		if (p_ptr->muta3 & MUT3_ALBINO)
		{
			SET_FLAG(of_ptr, TR_RES_DARK);
		}

		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
			of_ptr->flags[2] &= ~(TR2_REGEN);
			SET_FLAG(of_ptr, TR_HOLD_LIFE);
		}

		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
			SET_FLAG(of_ptr, TR_SEE_INVIS);
		}

#if 0
		if ((p_ptr->muta3 & MUT3_XTRA_FAT) ||
			(p_ptr->muta3 & MUT3_XTRA_LEGS) || (p_ptr->muta3 & MUT3_SHORT_LEG))
		{
			SET_FLAG(of_ptr, TR_SPEED);
		}
#endif

		if (p_ptr->muta3 & MUT3_ELEC_TOUC)
		{
			SET_FLAG(of_ptr, TR_SH_ELEC);
		}

		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
			SET_FLAG(of_ptr, TR_SH_FIRE);
			SET_FLAG(of_ptr, TR_LITE);
		}

		if (p_ptr->muta3 & MUT3_WINGS)
		{
			SET_FLAG(of_ptr, TR_FEATHER);
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			SET_FLAG(of_ptr, TR_RES_FEAR);
		}

		if (p_ptr->muta3 & MUT3_REGEN)
		{
			SET_FLAG(of_ptr, TR_REGEN);
		}

		if (p_ptr->muta3 & MUT3_ESP)
		{
			SET_FLAG(of_ptr, TR_TELEPATHY);
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			SET_FLAG(of_ptr, TR_FREE_ACT);
		}
		
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
			SET_FLAG(of_ptr, TR_HURT_ACID);
			SET_FLAG(of_ptr, TR_HURT_ELEC);
			SET_FLAG(of_ptr, TR_HURT_FIRE);
			SET_FLAG(of_ptr, TR_HURT_COLD);
		}
	}
}


/*
 * Equippy chars
 */
static void display_player_equippy(int x, int y)
{
	int i;

	byte a;
	char c;

	object_type *o_ptr;


	/* Dump equippy chars */
	for (i = 0; i < EQUIP_MAX; i++)
	{
		/* Object */
		o_ptr = &p_ptr->equipment[i];

		a = object_attr(o_ptr);
		c = object_char(o_ptr);

		/* No color */
		if (!use_color) a = TERM_WHITE;

		/* Clear the part of the screen */
		if (!o_ptr->k_idx)
		{
			c = ' ';
			a = TERM_DARK;
		}

		/* Dump */
		Term_putch(x + i, y, a, c);
	}
}


void print_equippy(void)
{
	display_player_equippy(COL_EQUIPPY, ROW_EQUIPPY);
}


/*
 * Helper function, see below
 */
static void display_player_flag_aux3(int col, int row,
                                    cptr header, int n1, u32b flag1, 
				    int n2, u32b flag2, int n3, u32b flag3)
{
	int i;
	object_flags oflags;

	/* Header */
	put_fstr(col, row, header);

	/* Advance */
	col += strlen(header) + 1;


	/* Check equipment */
	for (i = 0; i < EQUIP_MAX; i++)
	{
		object_type *o_ptr;

		/* Object */
		o_ptr = &p_ptr->equipment[i];

		/* Known flags */
		object_flags_known(o_ptr, &oflags);

		/* Default */
		put_fstr(col, row, CLR_SLATE ".");

		/* Check flags */
		if (oflags.flags[n3] & flag3)
		{
			put_fstr(col, row, CLR_RED "v");
			if (oflags.flags[n1] & flag1) put_fstr(col, row, CLR_RED "+");
			if (oflags.flags[n2] & flag2) put_fstr(col, row, CLR_RED "*");
		}
		else
		{
			if (oflags.flags[n1] & flag1) put_fstr(col, row, "+");
			if (oflags.flags[n2] & flag2) put_fstr(col, row, "*");
		}

		/* Advance */
		col++;
	}

	/* Player flags */
	player_flags(&oflags);

	/* Default */
	put_fstr(col, row, CLR_SLATE ".");

	/* Check flags */
	if (oflags.flags[n3] & flag3)
	{
		put_fstr(col, row, CLR_RED "v");
		if (oflags.flags[n1] & flag1) put_fstr(col, row, CLR_RED "+");
		if (oflags.flags[n2] & flag2) put_fstr(col, row, CLR_RED "*");
	}
	else
	{
		if (oflags.flags[n1] & flag1) put_fstr(col, row, "+");
		if (oflags.flags[n2] & flag2) put_fstr(col, row, "*");
	}
}

static void display_player_flag_aux2(int col, int row,
                                    cptr header, int n1, u32b flag1, 
				    int n2, u32b flag2)
{
	display_player_flag_aux3(col, row, header, n1, flag1, n2, flag2, 1, 0);
}

static void display_player_flag_aux(int col, int row,
                                    cptr header, int n1, u32b flag1)
{
	display_player_flag_aux3(col, row, header, n1, flag1, 1, 0, 1, 0);
}

/*
 * Special display, part 1
 */
static void display_player_flag_info(void)
{
	int row;
	int col;


	/*** Set 1 ***/

	row = 1;
	col = 0;

	display_player_equippy(col + 8, row++);

	put_fstr(col + 8, row++, "abcdefghijkl@");

	display_player_flag_aux3(col, row++, "Acid  :", TR_RES_ACID,
							TR_IM_ACID,
							TR_HURT_ACID);
	display_player_flag_aux3(col, row++, "Elec  :", TR_RES_ELEC,
							TR_IM_ELEC,
							TR_HURT_ELEC);
	display_player_flag_aux3(col, row++, "Fire  :", TR_RES_FIRE,
							TR_IM_FIRE,
							TR_HURT_FIRE);
	display_player_flag_aux3(col, row++, "Cold  :", TR_RES_COLD,
							TR_IM_COLD,
							TR_HURT_COLD);
	display_player_flag_aux2(col, row++, "Poison:", TR_RES_POIS,
							TR_IM_POIS);
	display_player_flag_aux(col, row++, "Fear  :",  TR_RES_FEAR);
	display_player_flag_aux3(col, row++, "Light :", TR_RES_LITE,
							TR_IM_LITE,
							TR_HURT_LITE);
	display_player_flag_aux3(col, row++, "Dark  :", TR_RES_DARK,
							TR_IM_DARK,
							TR_HURT_DARK);
	display_player_flag_aux(col, row++, "Shard :",  TR_RES_SHARDS);
	display_player_flag_aux(col, row++, "Blind :",  TR_RES_BLIND);
	display_player_flag_aux(col, row++, "Conf  :",  TR_RES_CONF);
	display_player_flag_aux(col, row++, "Sound :",  TR_RES_SOUND);
	display_player_flag_aux(col, row++, "Nether:",  TR_RES_NETHER);
	display_player_flag_aux(col, row++, "Nexus :",  TR_RES_NEXUS);
	display_player_flag_aux(col, row++, "Chaos :",  TR_RES_CHAOS);
	display_player_flag_aux(col, row++, "Disnch:",  TR_RES_DISEN);


	/*** Set 2 ***/

	row = 1;
	col = 24;

	display_player_equippy(col + 11, row++);

	put_fstr(col + 11, row++, "abcdefghijkl@");

	display_player_flag_aux(col, row++, "Reflect  :", TR_REFLECT);
	display_player_flag_aux(col, row++, "Aura Acid:", TR_SH_ACID);
	display_player_flag_aux(col, row++, "Aura Fire:", TR_SH_FIRE);
	display_player_flag_aux(col, row++, "Aura Elec:", TR_SH_ELEC);
	display_player_flag_aux(col, row++, "Aura Cold:", TR_SH_COLD);
	display_player_flag_aux(col, row++, "No Magic :", TR_NO_MAGIC);
	display_player_flag_aux(col, row++, "Free Actn:", TR_FREE_ACT);
	display_player_flag_aux(col, row++, "SeeInvis.:", TR_SEE_INVIS);
	display_player_flag_aux(col, row++, "Hold Life:", TR_HOLD_LIFE);
	display_player_flag_aux(col, row++, "Telepathy:", TR_TELEPATHY);
	display_player_flag_aux(col, row++, "SlwDigstn:", TR_SLOW_DIGEST);
	display_player_flag_aux(col, row++, "Regen.   :", TR_REGEN);
	display_player_flag_aux(col, row++, "Levitate :", TR_FEATHER);
	display_player_flag_aux(col, row++, "PermLite :", TR_LITE);
	display_player_flag_aux(col, row++, "Mutate   :", TR_MUTATE);
	display_player_flag_aux(col, row++, "Patron   :", TR_PATRON);
	display_player_flag_aux(col, row++, "Good Luck:", TR_LUCK_10);
	display_player_flag_aux(col, row++, "WeirdLuck:", TR_STRANGE_LUCK);
	display_player_flag_aux(col, row++, "Pass Wall:", TR_PASS_WALL);
	display_player_flag_aux(col, row++, "GhulTouch:", TR_GHOUL_TOUCH);

	/*** Set 3 ***/

	row = 1;
	col = 52;

	display_player_equippy(col + 11, row++);

	put_fstr(col + 11, row++, "abcdefghijkl@");

	display_player_flag_aux(col, row++, "Pr Animal:", TR_SLAY_ANIMAL);
	display_player_flag_aux(col, row++, "Pr Evil  :", TR_SLAY_EVIL);
	display_player_flag_aux(col, row++, "Pr Undead:", TR_SLAY_UNDEAD);
	display_player_flag_aux(col, row++, "Pr Demon :", TR_SLAY_DEMON);
	display_player_flag_aux(col, row++, "Pr Orc   :", TR_SLAY_ORC);
	display_player_flag_aux(col, row++, "Pr Troll :", TR_SLAY_TROLL);
	display_player_flag_aux(col, row++, "Pr Giant :", TR_SLAY_GIANT);
	display_player_flag_aux(col, row++, "Pr Dragon:", TR_SLAY_DRAGON);

	display_player_flag_aux2(col, row++, "Cursed   :", TR_CURSED,
							2, TR2_HEAVY_CURSE | TR2_PERMA_CURSE);
	display_player_flag_aux(col, row++, "AutoCurse:", TR_AUTO_CURSE);
	display_player_flag_aux(col, row++, "Teleport :", TR_TELEPORT);
	display_player_flag_aux(col, row++, "NoTeleprt:", TR_NO_TELE);
	display_player_flag_aux(col, row++, "Aggravate:", TR_AGGRAVATE);
	display_player_flag_aux(col, row++, "DrainStat:", TR_DRAIN_STATS);
	display_player_flag_aux(col, row++, "Drain Exp:", TR_DRAIN_EXP);
	display_player_flag_aux(col, row++, "Slow Heal:", TR_SLOW_HEAL);
	display_player_flag_aux(col, row++, "Can't Eat:", TR_CANT_EAT);
	display_player_flag_aux(col, row++, "EvilCurse:", TR_TY_CURSE);
}


/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
static void display_player_stat_info(void)
{
	int i, e_adj, r_adj, c_adj;
	int stat_col, stat;
	int row, col;

	object_type *o_ptr;
	object_flags oflags;
	s16b k_idx;

	byte a;
	char c;

	/* Column */
	stat_col = 16;

	/* Row */
	row = 3;

	/* Print out the labels for the columns */
	put_fstr(stat_col, row - 1, "Stat");
	put_fstr(stat_col + 5, row - 1, CLR_BLUE "Intrnl");
	put_fstr(stat_col + 12, row - 1, CLR_L_BLUE "Rce Cls Mod");
	put_fstr(stat_col + 24, row - 1, CLR_L_GREEN "Actual");
	put_fstr(stat_col + 31, row - 1, CLR_YELLOW "Currnt");

	/* Display the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Calculate equipment adjustment */
		e_adj = p_ptr->stat[i].top - p_ptr->stat[i].max;

		/* Get race and class adjustments */
		r_adj = rp_ptr->r_adj[i] * 10;
		c_adj = cp_ptr->c_adj[i] * 10;

		/* Reduced name of stat */
		put_fstr(stat_col, row + i, stat_names_reduced[i]);

		/* Internal "natural" max value.  Maxes at 40 */
		/* This is useful to see if you are maxed out */
		/* We actually fake this by subtracting out the race/class bonuses... */
		put_fstr(stat_col + 5, row + i, CLR_BLUE "%v",
				 stat_format, p_ptr->stat[i].max - r_adj - c_adj);

		/* Race, class, and equipment modifiers */
		put_fstr(stat_col + 12, row + i, CLR_L_BLUE "%3d", (int)(r_adj / 10));
		put_fstr(stat_col + 16, row + i, CLR_L_BLUE "%3d", (int)(c_adj / 10));
		put_fstr(stat_col + 20, row + i, CLR_L_BLUE "%3d", (int)(e_adj / 10));

		/* Actual maximal modified value */
		put_fstr(stat_col + 24, row + i, CLR_L_GREEN "%v",
				 stat_format, p_ptr->stat[i].top);

		/* Only display stat_use if not maximal */
		if (p_ptr->stat[i].use < p_ptr->stat[i].top)
		{
			put_fstr(stat_col + 31, row + i, CLR_YELLOW "%v",
						 stat_format, p_ptr->stat[i].use);
		}
	}

	/* Column */
	col = stat_col + 39;

	/* Header and Footer */
	display_player_equippy(col, row - 2);
	put_fstr(col, row - 1, "abcdefghijkl@");
	put_fstr(col, row + 6, CLR_L_GREEN "Modifications");

	/* Process equipment */
	for (i = 0; i < EQUIP_MAX; i++)
	{
		/* Access object */
		o_ptr = &p_ptr->equipment[i];

		/* Object kind */
		k_idx = o_ptr->k_idx;

		/* Acquire "known" flags */
		object_flags_known(o_ptr, &oflags);

		/* Initialize color based of sign of pval. */
		for (stat = 0; stat < A_MAX; stat++)
		{
			/* Default */
			a = TERM_SLATE;
			c = '.';

			/* Boost */
			if (oflags.flags[0] & (1 << stat))
			{
				/* Default */
				c = '*';

				/* Good */
				if (o_ptr->pval > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				if (oflags.flags[1] & (1 << stat))
				{
					/* Dark green for sustained stats. */
					a = TERM_GREEN;
				}

				/* Bad */
				if (o_ptr->pval < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (-o_ptr->pval < 10) c = '0' - o_ptr->pval;
				}
			}

			/* Sustain */
			else if (oflags.flags[1] & (1 << stat))
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Handle monochrome */
			if (!use_color) a = TERM_WHITE;

			/* Dump proper character */
			Term_putch(col, row + stat, a, c);
		}

		/* Advance */
		col++;
	}

	/* Player flags */
	player_flags(&oflags);

	/* Check stats */
	for (stat = 0; stat < A_MAX; stat++)
	{
		/* Default */
		a = TERM_SLATE;
		c = '.';

		/* Sustain */
		if (oflags.flags[1] & 1 << stat)
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}


		/* Mutations ... */
		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
		{
			int dummy = 0;

			if (stat == A_STR)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (p_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (p_ptr->muta1 & MUT1_MIND_BLST) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy -= 1;
			}
			else if (stat == A_INT)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta1 & MUT1_STERILITY) dummy -= 1;
				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy -= 1;
			}
			else if (stat == A_WIS)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->muta1 & MUT1_HYPN_GAZE) dummy -= 1;
				if (p_ptr->muta1 & MUT1_BERSERK) dummy -= 1;
				if (p_ptr->muta1 & MUT1_MIDAS_TCH) dummy -= 1;
				if (p_ptr->muta1 & MUT1_EARTHQUAKE) dummy -= 1;
				if (p_ptr->muta2 & MUT2_INVULN) dummy -= 2;
				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy -= 1;
			}
			else if (stat == A_DEX)
			{
				if (p_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (p_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
				if (p_ptr->muta1 & MUT1_COLD_TOUCH) dummy -= 1;
				if (p_ptr->muta1 & MUT1_LAUNCHER) dummy -= 1;
				if (p_ptr->muta2 & MUT2_TENTACLES) dummy += 1;
				if (p_ptr->muta3 & MUT3_PUNY) dummy += 2;
				if (p_ptr->muta3 & MUT3_XTRA_LEGS) dummy -= 1;
			}
			else if (stat == A_CON)
			{
				if (p_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (p_ptr->muta3 & MUT3_XTRA_FAT) dummy += 2;
				if (p_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 2;
				if (p_ptr->muta1 & MUT1_HYPN_GAZE) dummy -= 1;
				if (p_ptr->muta1 & MUT1_RADIATION) dummy -= 1;
				if (p_ptr->muta3 & MUT3_SHORT_LEG) dummy += 1;
				if (p_ptr->muta2 & MUT2_WRAITH) dummy -= 2;
			}
			else if (stat == A_CHR)
			{
				if (p_ptr->muta3 & MUT3_SILLY_VOI) dummy -= 4;
				if (p_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 1;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 1;
				if (p_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				if (p_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				if (p_ptr->muta1 & MUT1_SHRIEK) dummy -= 1;
				if (p_ptr->muta1 & MUT1_MIDAS_TCH) dummy -= 1;
				if (p_ptr->muta2 & MUT2_SCOR_TAIL) dummy -= 2;
				if (p_ptr->muta2 & MUT2_TRUNK) dummy -= 1;
				if (p_ptr->muta2 & MUT2_TENTACLES) dummy -= 1;
				if (p_ptr->muta3 & MUT3_XTRA_EYES) dummy -= 1;
				if (p_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
			}

			/* Boost */
			if (dummy)
			{
				/* Default */
				c = '*';

				/* Good */
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				/* Sustains */
				if (oflags.flags[1] & (1 << stat))
				{
					a = TERM_GREEN;
				}

				/* Bad */
				if (dummy < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (-dummy < 10) c = '0' - dummy;
				}
			}
		}


		/* No color */
		if (!use_color) a = TERM_WHITE;

		/* Dump */
		Term_putch(col, row + stat, a, c);
	}
}

static void display_player_skill_info(void)
{
	int row, col, skill_col;
	int i;
	int skill;

	object_type *o_ptr;
	object_flags oflags;
	s16b k_idx;

	byte a;
	char c;

	row = 13;
	skill_col = 45;

	put_fstr(skill_col, row - 1, "Skill");

	put_fstr(skill_col, row,     "BonusSP:");
	put_fstr(skill_col, row + 1, "Stealth:");
	put_fstr(skill_col, row + 2, "Search :");
	put_fstr(skill_col, row + 3, "Infra  :");
	put_fstr(skill_col, row + 4, "Tunnel :");
	put_fstr(skill_col, row + 5, "Speed  :");
	put_fstr(skill_col, row + 6, "Blows  :");

	/* Column */
	col = skill_col + 10;

	/* Header and Footer */
	display_player_equippy(col, row - 2);
	put_fstr(col, row - 1, "abcdefghijkl@");
	put_fstr(col, row + 6, CLR_L_GREEN "Modifications");

	/* Process equipment */
	for (i = 0; i < EQUIP_MAX; i++)
	{
		/* Access object */
		o_ptr = &p_ptr->equipment[i];

		/* Object kind */
		k_idx = o_ptr->k_idx;

		/* Acquire "known" flags */
		object_flags_known(o_ptr, &oflags);

		/* Initialize color based of sign of pval. */
		for (skill = 0; skill < 7; skill++)
		{
			/* Default */
			a = TERM_SLATE;
			c = '.';

			/* Boost */
			if (oflags.flags[0] & (TR0_SP << skill))
			{
				/* Default */
				c = '*';

				/* Good */
				if (o_ptr->pval > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				/* Bad */
				if (o_ptr->pval < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (-o_ptr->pval < 10) c = '0' - o_ptr->pval;
				}
			}

			/* Handle monochrome */
			if (!use_color) a = TERM_WHITE;

			/* Dump proper character */
			Term_putch(col, row + skill, a, c);
		}

		/* Advance */
		col++;
	}

	/* Player flags */
	player_flags(&oflags);

	for (skill = 0; skill < 7; skill++)
	{
		int dummy = 0;

		/* Default */
		a = TERM_SLATE;
		c = '.';

		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
		{
			/* Stealth */
			if (skill == 1)
			{
				if (p_ptr->muta1 & MUT1_ILLUMINE) dummy -= 1;
				if (p_ptr->muta1 & MUT1_DAZZLE) dummy -= 1;
				if (p_ptr->muta3 & MUT3_XTRA_NOIS) dummy -= 3;
				if (p_ptr->muta3 & MUT3_MOTION) dummy += 1;
			}
			/* Search */
			else if (skill == 2)
			{
				if (p_ptr->muta1 & MUT1_LASER_EYE) dummy -= 2;
				if (p_ptr->muta3 & MUT3_XTRA_EYES) dummy += 3;
			}
			/* Infra */
			else if (skill == 3)
			{
				if (p_ptr->muta3 & MUT3_INFRAVIS) dummy += 3;
			}
			/* Speed */
			else if (skill == 5)
			{
				if (p_ptr->muta3 & MUT3_XTRA_LEGS) dummy += 3;
				if (p_ptr->muta3 & MUT3_SHORT_LEG) dummy -= 3;
			}
		}
				
		if (skill == 5 && (FLAG(&oflags, TR_SPEED)))
		{
			dummy += p_ptr->lev / 10;
		}

		/* Boost */
		if (dummy)
		{
			/* Default */
			c = '*';

			/* Good */
			if (dummy > 0)
			{
				/* Good */
				a = TERM_L_GREEN;

				/* Label boost */
				if (dummy < 10) c = '0' + dummy;
			}

			/* Bad */
			if (dummy < 0)
			{
				/* Bad */
				a = TERM_RED;

				/* Label boost */
				if (-dummy < 10) c = '0' - dummy;
			}
		}

		/* No color */
		if (!use_color) a = TERM_WHITE;

		/* Dump */
		Term_putch(col, row + skill, a, c);
	}

}

#define COL_NAME			0
#define WID_NAME			11

#define COL_AGE				32
#define COL_STATS			55


/*
 * Display the standard player information
 */
static void display_player_top(void)
{
	int i;
	
	/* Name, Sex, Race, Class */
	put_fstr(COL_NAME, 2,
    			"Name     : " CLR_L_BLUE "%s\n" CLR_WHITE
				"Sex      : " CLR_L_BLUE "%s\n" CLR_WHITE
				"Race     : " CLR_L_BLUE "%s\n" CLR_WHITE
				"Class    : " CLR_L_BLUE "%s",
                player_name, sp_ptr->title, rp_ptr->title, cp_ptr->title);

	if (p_ptr->spell.r[0].realm || p_ptr->spell.r[1].realm)
	{
		put_fstr(COL_NAME, 6, "Magic    : " CLR_L_BLUE "%s", realm_names[p_ptr->spell.r[0].realm]);
	}

	if (p_ptr->spell.r[1].realm)
	{
		put_fstr(COL_NAME + WID_NAME, 7, CLR_L_BLUE "%s",
				 realm_names[p_ptr->spell.r[1].realm]);
	}
	else if (FLAG(p_ptr, TR_PATRON))
	{
		put_fstr(COL_NAME, 7, "Patron   : " CLR_L_BLUE "%s",
					 chaos_patrons[p_ptr->chaos_patron]);
	}

	/* Age, Height, Weight, Social */
	prt_num(COL_AGE, 2, "Age         " CLR_L_BLUE, (int)p_ptr->rp.age, 3);
	prt_num(COL_AGE, 3, "Height      " CLR_L_BLUE, (int)p_ptr->rp.ht, 3);
	prt_num(COL_AGE, 4, "Weight      " CLR_L_BLUE, (int)p_ptr->rp.wt, 3);
	prt_num(COL_AGE, 5, "Social Class" CLR_L_BLUE, (int)p_ptr->rp.sc, 3);

	/* Display the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Special treatment of "injured" stats */
		if (p_ptr->stat[i].cur < p_ptr->stat[i].max)
		{
			/* Use lowercase stat name */
			put_fstr(COL_STATS, i + 2, stat_names_reduced[i]);

			/* Display the current stat (modified) */
			put_fstr(COL_STATS + 5, i + 2, CLR_YELLOW "%v",
					 stat_format, p_ptr->stat[i].use);

			/* Display the maximum stat (modified) */
			put_fstr(COL_STATS + 5 + 7, i + 2, CLR_L_GREEN "%v",
					 stat_format, p_ptr->stat[i].top);
		}

		/* Normal treatment of "normal" stats */
		else
		{
			/* Assume uppercase stat name */
			put_fstr(COL_STATS, i + 2, stat_names[i]);

			/* Display the current stat (modified) */
			put_fstr(COL_STATS + 5, i + 2, CLR_L_GREEN "%v",
					 stat_format, p_ptr->stat[i].use);
		}
	}
}


#define COL_BONUS			0
#define COL_VALUE			23
#define COL_LIFE     		51


/*
 * Display the player attributes.
 */
static void display_player_middle(void)
{
	int percentdam;
	int show_tohit = p_ptr->dis_to_h;
	int show_todam = p_ptr->dis_to_d;

	object_type *o_ptr = &p_ptr->equipment[EQUIP_WIELD];

	/* Hack -- add in weapon info if known */
	if (object_known_p(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_known_p(o_ptr)) show_todam += o_ptr->to_d;

	/* convert to oangband "deadliness" */
	percentdam = deadliness_calc(show_todam);

	/*** Bonuses ***/

	prt_num(COL_BONUS, 9, "+ Skill     " CLR_L_BLUE, show_tohit, 3);
	prt_num(COL_BONUS, 10, "% Deadliness" CLR_L_BLUE, percentdam, 3);
	prt_num(COL_BONUS, 11, "+ To AC     " CLR_L_BLUE, p_ptr->dis_to_a, 3);
	prt_num(COL_BONUS, 12, "  Base AC   " CLR_L_BLUE, p_ptr->dis_ac, 3);


	/*** Level, experience, gold ***/

	prt_num(COL_VALUE, 9, "Level      " CLR_L_GREEN, (int)p_ptr->lev, 9);

	if (p_ptr->exp >= p_ptr->max_exp)
	{
		prt_num(COL_VALUE, 10, "Experience " CLR_L_GREEN, p_ptr->exp, 9);
	}
	else
	{
		prt_num(COL_VALUE, 10, "Experience " CLR_YELLOW, p_ptr->exp, 9);
	}

	prt_num(COL_VALUE, 11, "Max Exp    " CLR_L_GREEN, p_ptr->max_exp, 9);

	if (p_ptr->lev >= PY_MAX_LEVEL)
	{
		put_fstr(COL_VALUE, 12, "Exp to Adv." CLR_L_GREEN "       *****");
	}
	else if (toggle_xp)
	{
		/* Print the amount of xp until next level */
		prt_num(COL_VALUE, 12, "Exp to Adv." CLR_L_GREEN,
				(long)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L
					   - (long)p_ptr->exp), 9);
	}
	else
	{
		/* Print the total xp required for next level */
		prt_num(COL_VALUE, 12, "Exp to Adv." CLR_L_GREEN,
				(long)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L), 9);
	}


	prt_num(COL_VALUE, 13, "Gold       " CLR_L_GREEN, p_ptr->au, 9);


	/*** Hitpoints/spellpoints ***/

	prt_num(COL_LIFE, 9, "Max Hit Points" CLR_L_GREEN, p_ptr->mhp, 5);

	if (p_ptr->chp >= p_ptr->mhp)
	{
		prt_num(COL_LIFE, 10, "Cur Hit Points" CLR_L_GREEN, p_ptr->chp, 5);
	}
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
	{
		prt_num(COL_LIFE, 10, "Cur Hit Points" CLR_YELLOW, p_ptr->chp, 5);
	}
	else
	{
		prt_num(COL_LIFE, 10, "Cur Hit Points" CLR_RED, p_ptr->chp, 5);
	}

	prt_num(COL_LIFE, 11, "Max SP (Mana) " CLR_L_GREEN, p_ptr->msp, 5);

	if (p_ptr->csp >= p_ptr->msp)
	{
		prt_num(COL_LIFE, 12, "Cur SP (Mana) " CLR_L_GREEN, p_ptr->csp, 5);
	}
	else if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10)
	{
		prt_num(COL_LIFE, 12, "Cur SP (Mana) " CLR_YELLOW, p_ptr->csp, 5);
	}
	else
	{
		prt_num(COL_LIFE, 12, "Cur SP (Mana) " CLR_RED, p_ptr->csp, 5);
	}
}


/*
 * Display the standard player information and abilities
 */
static void display_player_standard(void)
{
	/* Basic info */
	display_player_top();

	/* Extra info */
	display_player_middle();

	put_fstr(25, 15, "(Miscellaneous Abilities)");

	/* Display the abilities */
	display_player_abilities();
}


/*
 * Display a summary of the player's attributes
 *
 * See "http://www.cs.berkeley.edu/~davidb/angband.html"
 */
static void display_player_flag(void)
{
	/* Dump the info */
	display_player_flag_info();
}

/*
 * Display a summary of the player's bonuses
 */
static void display_player_stat(void)
{
	/* Dump the info */
	display_player_stat_info();
	display_player_skill_info();
}


typedef void (*display_func) (void);


static display_func displays[DISPLAY_PLAYER_MAX] =
{
	/* Standard display with skills */
	display_player_standard,

	/* Summary of various things */
	display_player_flag,

	/* Summary of stat & skill boosts */
	display_player_stat,
};


/*
 * Display the character on the screen (various modes)
 *
 * The top two and bottom two lines are left blank.
 */
void display_player(int mode)
{
	mode %= DISPLAY_PLAYER_MAX;

	/* Erase screen */
	clear_from(0);

	/* Display it */
	(*(displays[mode])) ();
}


/*
 * Hack -- change name
 */
void do_cmd_character(void)
{
	char c;

	int mode = DISPLAY_PLAYER_STANDARD;

	char tmp[160];

	/* Save the screen */
	screen_save();

	/* Forever */
	while (1)
	{
		/* Display the player */
		display_player(mode);

		/* Prompt */
		put_fstr(2, 23,
					"['c' to change name, 'f' to file, 'p' for previous, 'n' for next, or ESC]");

		/* Query */
		c = inkey();

		/* Exit */
		if (c == ESCAPE) break;

		/* Change name */
		if (c == 'c')
		{
			change_player_name();
		}

		/* File dump */
		else if (c == 'f')
		{
			strnfmt(tmp, 160, "%s.txt", player_base);
			if (get_string(tmp, 80, "File name: "))
			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					(void)file_character(tmp, TRUE);
				}
			}
		}

		/* Decrease mode */
		else if (c == 'p')
		{
			mode = (mode + DISPLAY_PLAYER_MAX - 1) % DISPLAY_PLAYER_MAX;
		}

		/* Increase mode */
		else if (c == 'n')
		{
			mode = (mode + 1) % DISPLAY_PLAYER_MAX;
		}

		/* Oops */
		else
		{
			bell("Illegal option!");
		}

		/* Flush messages */
		message_flush();
	}

	/* Restore the screen */
	screen_load();

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	handle_stuff();
}


/*
 * Hack -- Dump a character description file
 *
 * XXX XXX XXX Allow the "full" flag to dump additional info,
 * and trigger its usage from various places in the code.
 */
errr file_character(cptr name, bool full)
{
	int i, j, k, x, y;
	byte a;
	char c;
	cptr paren = ")";
	int fd = -1;
	FILE *fff = NULL;
	store_type *st_ptr;
	char buf[1024];

	int msg_max = message_num();

	object_type *o_ptr;


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		/* Close the file */
		(void)fd_close(fd);

		/* Ask */
		if (get_check("Replace existing file %s? ", buf)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* Invalid file */
	if (!fff)
	{
		/* Message */
		msgf("Character dump failed!");
		message_flush();

		/* Error */
		return (-1);
	}


	froff(fff, "  [%s %s Character Dump]\n\n", VERSION_NAME, VERSION_STRING);

	/* Display player */
	display_player(DISPLAY_PLAYER_STANDARD);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x - 1] == ' ')) buf[--x] = '\0';

		/* End the row */
		froff(fff, "%s\n", buf);
	}

	froff(fff, "\n\n  [Miscellaneous information]\n");

	if (preserve_mode)
		froff(fff, "\n Preserve Mode:      ON");
	else
		froff(fff, "\n Preserve Mode:      OFF");

	if (ironman_small_levels)
		froff(fff, "\n Small Levels:       ALWAYS");
	else if (small_levels)
		froff(fff, "\n Small Levels:       ON");
	else
		froff(fff, "\n Small Levels:       OFF");

	if (vanilla_town) froff(fff, "\n Vanilla Town:       ON");

	if (ironman_shops) froff(fff, "\n No Shops:           ON");

	if (ironman_downward) froff(fff, "\n Diving only:        ON");

	if (ironman_nightmare) froff(fff, "\n Nightmare Mode:     ON");

	froff(fff, "\n Recall Depth:       Level %d (%d')\n",
		 	max_dun_level_reached(), 50 * max_dun_level_reached());

	if (p_ptr->state.noscore)
		froff(fff, "\n You have done something illegal.");

	if (stupid_monsters)
		froff(fff, "\n Your opponents are behaving stupidly.");

	if (munchkin_death)
		froff(fff, "\n You possess munchkinish power over death.");

	/* Show (known) flags grid */
	if (full)
	{
		/* New line */
		froff(fff, "\n\n");
	
		display_player(DISPLAY_PLAYER_FLAG);
		
		/* Dump part of the screen */
		for (y = 2; y < 9; y++)
		{
			/* Dump each row */
			for (x = 0; x < 63; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x + 16, y, &a, &c));
	
				/* Dump it */
				buf[x] = c;
			}

			/* End the string */
			buf[x] = '\0';

			/* Kill trailing spaces */
			while ((x > 0) && (buf[x - 1] == ' ')) buf[--x] = '\0';

			/* End the row */
			froff(fff, "%s\n", buf);
		}
		
		/* New line */
		froff(fff, "\n");

        /* Dump column */
		for (y = 12; y < 20; y++)
		{
			for (x = 0; x < 24; x++)
			{
				(void)(Term_what(x + 45, y, &a, &c));
				buf[x] = c;
			}

			buf[x] = '\0';
			froff(fff, "%s\n", buf);
		}
	
		froff(fff, "\n");
		display_player(DISPLAY_PLAYER_SUMMARY);

		/* Dump first column */
		for (y = 2; y < 19; y++)
		{
			for (x = 0; x < 21; x++)
			{
				(void)(Term_what(x, y, &a, &c));
				buf[x] = c;
			}

			buf[x] = '\0';
			froff(fff, "%s\n", buf);
		}

		/* New line */
		froff(fff, "\n");

		/* Dump second column */
		for (y = 2; y < 23; y++)
		{
			for (x = 0; x < 24; x++)
			{
				(void)(Term_what(x + 24, y, &a, &c));
				buf[x] = c;
			}

			buf[x] = '\0';
			froff(fff, "%s\n", buf);
		}

		/* New line */
		froff(fff, "\n");

		/* Dump third column */
		for (y = 2; y < 22; y++)
		{
			for (x = 0; x < 24; x++)
			{
				(void)(Term_what(x + 52, y, &a, &c));
				buf[x] = c;
			}

			buf[x] = '\0';
			froff(fff, "%s\n", buf);
		}
	}

	/* Monsters slain */
	{
		u32b Total = 0;

		for (k = 1; k < z_info->r_max; k++)
		{
			monster_race *r_ptr = &r_info[k];

			if (FLAG(r_ptr, RF_UNIQUE))
			{
				if (r_ptr->max_num == 0) Total++;
			}
			else
			{
				Total += r_ptr->r_pkills;
			}
		}

		if (Total < 1)
			froff(fff, "\n You have defeated no enemies yet.\n");
		else if (Total == 1)
			froff(fff, "\n You have defeated one enemy.\n");
		else
			froff(fff, "\n You have defeated %lu enemies.\n", Total);
	}

    /* Top kills */
    {
        u16b *who;
        int n;
        u16b why = 0;

        /* Allocate the "who" array */
        C_MAKE(who, z_info->r_max, u16b);

        /* Collect matching monsters */
        for (n = 0, i = 1; i < z_info->r_max; i++)
        {
            monster_race *r_ptr = &r_info[i];
    
            /* Require killed monsters */
            if (!r_ptr->r_pkills) continue;

            /* Collect monsters */
            who[n++] = i;
        }

        if (n)
        {
            froff(fff, "\n\n  [Top %i deepest kills]\n\n", n >= 10 ? 10 : n);

            why = 2;
        
            /* Select the sort method */
            ang_sort_comp = ang_sort_comp_hook;
            ang_sort_swap = ang_sort_swap_hook;
        
            /* Sort the array */
            ang_sort(who, &why, n);

            for (i = n - 1; i >= 0 && i >= n - 10; i--)
            {
                monster_race *r_ptr = &r_info[who[i]];

                froff(fff, "%2i %-36s Level %i (%i')\n", r_ptr->r_pkills,
                        (r_ptr->name + r_name), r_ptr->level, r_ptr->level * 50);
            }
        }

        /* Free the "who" array */
        KILL(who);
    }

#if 0
	froff(fff, "\n\n  [Virtues]\n\n");
	dump_virtues(fff);
#endif /* 0 */

	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
	{
		froff(fff, "\n\n  [Mutations]\n\n");
		dump_mutations(fff);
	}


	/* Skip some lines */
	froff(fff, "\n\n");


	/* Dump the equipment */
	froff(fff, "  [Character Equipment]\n\n");
	for (i = 0; i < EQUIP_MAX; i++)
	{
		froff(fff, "%c%s %v\n", I2A(i), paren,
				OBJECT_FMT(&p_ptr->equipment[i], TRUE, 3));
	}
	froff(fff, "\n\n");


	/* Dump the inventory */
	froff(fff, "  [Character Inventory]\n\n");

	i = 0;

	OBJ_ITT_START (p_ptr->inventory, o_ptr)
	{
		/* Dump the inventory slots */
		froff(fff, "%c%s %v\n", I2A(i), paren, OBJECT_FMT(o_ptr, TRUE, 3));

		/* Count slots */
		i++;
	}
	OBJ_ITT_END;

	/* Add an empty line */
	froff(fff, "\n\n");

	/* Print all homes in the different towns */
	for (i = 1; i < z_info->wp_max; i++)
	{
		for (j = 0; j < place[i].numstores; j++)
		{
			st_ptr = &place[i].store[j];

			if (st_ptr->type == BUILD_STORE_HOME)
			{
				/* Home -- if anything there */
				if (st_ptr->stock)
				{
					/* Header with name of the town */
					froff(fff, "  [Home Inventory - %s]\n\n", place[i].name);

					/* Initialise counter */
					k = 0;

					/* Dump all available items */
					OBJ_ITT_START (st_ptr->stock, o_ptr)
					{
						froff(fff, "%c%s %v\n", I2A(k), paren,
								OBJECT_FMT(o_ptr, TRUE, 3));

						/* Increment counter */
						k++;
					}
					OBJ_ITT_END;

					/* Add an empty line */
					froff(fff, "\n\n");
				}
			}
		}
	}

	if (limit_messages && msg_max > 50)
		msg_max = 50;

	froff(fff, "  [Message Log (last %d messages)]\n\n", msg_max);

	for (i = msg_max - 1; i >= 0; i--)
	{
		froff(fff, "%s\n", message_str((s16b)i));
	}
	froff(fff, "\n\n");


	/* Close it */
	my_fclose(fff);


	/* Message */
	msgf("Character dump successful.");
	message_flush();

	/* Success */
	return (0);
}


/*
 * Recursive file perusal.
 *
 * Return FALSE on "ESCAPE", otherwise TRUE.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * XXX XXX XXX Consider using a temporary file.
 *
 * XXX XXX XXX Allow the user to "save" the current file.
 */
bool show_file(cptr name, cptr what, int line, int mode)
{
	int i, n, k;
	int wid, hgt;

	/* Number of "real" lines passed by */
	int next = 0;

	/* Number of "real" lines in the file */
	int size = 0;

	/* Backup value for "line" */
	int back = 0;

	/* Loop counter */
	int cnt;

	/* This screen has sub-screens */
	bool menu = FALSE;

	/* Current help file */
	FILE *fff = NULL;

	/* Find this string (if any) */
	cptr find = NULL;

	/* Jump to this tag */
	cptr tag = NULL;

	/* Hold a string to find */
	char finder[81];

	/* Hold a string to show */
	char shower[81];

	/* Filename */
	char filename[1024];

	/* Describe this thing */
	char caption[128];

	/* Path buffer */
	char path[1024];

	/* General buffer */
	char buf[1024];

	/* Lower case version of the buffer, for searching */
	char lc_buf[1024];

	/* Aux pointer for making lc_buf (and find!) lowercase */
	cptr lc_buf_ptr;

	/* Sub-menu information */
	char hook[62][32];


	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Wipe finder */
	finder[0] = 0;

	/* Wipe shower */
	shower[0] = 0;

	/* Wipe caption */
	caption[0] = 0;

	/* Wipe the hooks */
	for (i = 0; i < 62; i++)
	{
		hook[i][0] = '\0';
	}

	/* Copy the filename */
	strcpy(filename, name);

	n = strlen(filename);

	/* Extract the tag from the filename */
	for (i = 0; i < n; i++)
	{
		if (filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack XXX XXX XXX */
	if (what)
	{
		/* Caption */
		strcpy(caption, what);

		/* Access the "file" */
		strcpy(path, name);

		/* Open */
		fff = my_fopen(path, "r");
	}

	/* Look in "help" */
	if (!fff)
	{
		/* Caption */
		strnfmt(caption, 128, "Help file '%s'", name);

		/* Build the filename */
		path_build(path, 1024, ANGBAND_DIR_HELP, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Caption */
		strnfmt(caption, 128, "Info file '%s'", name);

		/* Build the filename */
		path_build(path, 1024, ANGBAND_DIR_INFO, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Oops */
	if (!fff)
	{
		/* Message */
		msgf("Cannot open '%s'.", name);
		message_flush();

		/* Oops */
		return (TRUE);
	}
	
	/* Pre-Parse the file */
	while (TRUE)
	{
		/* Read a line or stop */
		if (my_raw_fgets(fff, buf, 1024)) break;

		/* XXX Parse "menu" items */
		if (prefix(buf, "***** "))
		{
			/* Notice "menu" requests */
			if ((buf[6] == '[') && (isdigit(buf[7]) || isalpha(buf[7])))
			{
				/* This is a menu file */
				menu = TRUE;

				/* Extract the menu item */
				k = isdigit(buf[7]) ? D2I(buf[7]) : buf[7] - 'A' + 10;

				if ((buf[8] == ']') && (buf[9] == ' '))
				{
					/* Extract the menu item */
					strncpy(hook[k], buf + 10, 31);

					/* Make sure it's null-terminated */
					hook[k][31] = '\0';
				}
			}
			/* Notice "tag" requests */
			else if (buf[6] == '<')
			{
				if (tag)
				{
					/* Remove the closing '>' of the tag */
					buf[strlen(buf) - 1] = '\0';

					/* Compare with the requested tag */
					if (streq(buf + 7, tag))
					{
						/* Remember the tagged line */
						line = next;
					}
				}
			}

			/* Skip this */
			continue;
		}

		/* Count the "real" lines */
		next++;
	}
	
	screen_save();

	/* Save the number of "real" lines */
	size = next;

	/* Display the file */
	while (TRUE)
	{
		/* Clear screen */
		Term_clear();

		/* Restart when necessary */
		if (line >= size) line = 0;


		/* Re-open the file if needed */
		if (next > line)
		{
			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			/* Oops */
			if (!fff)
			{
				screen_load();
				return (FALSE);
			}
			
			/* File has been restarted */
			next = 0;
		}

		/* Goto the selected line */
		while (next < line)
		{
			/* Get a line */
			if (my_raw_fgets(fff, buf, 1024)) break;

			/* Skip tags/links */
			if (prefix(buf, "***** ")) continue;

			/* Count the lines */
			next++;
		}

		/* Dump the next hgt - 4 lines of the file */
		for (i = 0; i < hgt - 4;)
		{
			/* Hack -- track the "first" line */
			if (!i) line = next;

			/* Get a line of the file or stop */
			if (my_raw_fgets(fff, buf, 1024)) break;

			/* Hack -- skip "special" lines */
			if (prefix(buf, "***** ")) continue;

			/* Count the "real" lines */
			next++;

			/* Make a lower case version of buf for searching */
			strcpy(lc_buf, buf);

			for (lc_buf_ptr = lc_buf; *lc_buf_ptr != 0; lc_buf_ptr++)
			{
				lc_buf[lc_buf_ptr - lc_buf] = tolower(*lc_buf_ptr);
			}

			/* Hack -- keep searching */
			if (find && !i && !strstr(lc_buf, find)) continue;

			/* Hack -- stop searching */
			find = NULL;

			/* Dump the line */
			put_fstr(0, i + 2, "%s", buf);

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr str = lc_buf;

				/* Display matches */
				while ((str = strstr(str, shower)) != NULL)
				{
					int offset = fmt_offset(lc_buf, str);
				
					/* Display the match */
					put_fstr(offset, i + 2, CLR_YELLOW "%s", shower);

					/* Advance */
					str += strlen(shower);
				}
			}

			/* Count the printed lines */
			i++;
		}

		/* Hack -- failed search */
		if (find)
		{
			bell("Search string not found!");
			line = back;
			find = NULL;
			continue;
		}


		/* Show a general "title" */
		prtf(0, 0, "[%s %s, %s, Line %d/%d]",
				   VERSION_NAME, VERSION_STRING, caption, line, size);

		/* Prompt -- with menu */
		if (menu)
		{
			/* Prompt -- small files */
			if (size <= hgt - 4)
			{
				/* Wait for it */
				prtf(0, hgt - 1, "[Press a Number, or ESC to exit.]");
			}

			/* Prompt -- large files */
			else
			{
				/* Wait for it */
				prtf(0, hgt - 1,
				"[Press a Number, Return, Space, -, =, /, \\, or ESC to exit.]");
			}
		}
		else
		{
			/* Prompt -- small files */
			if (size <= hgt - 4)
			{
				/* Wait for it */
				prtf(0, hgt - 1, "[Press ESC to exit.]");
			}

			/* Prompt -- large files */
			else
			{
				/* Wait for it */
				prtf(0, hgt - 1, "[Press Return, Space, -, =, /, \\, or ESC to exit.]");
			}
		}

		/* Get a keypress */
		k = inkey();

		/* Hack -- return to last screen */
		if (k == '<') break;

		/* Show the help for the help */
		if (k == '?')
		{
			/* Hack - prevent silly recursion */
			if (!streq(name, "helpinfo.txt"))
				show_file("helpinfo.txt", NULL, 0, mode);
		}

		/* Hack -- try showing */
		if (k == '=')
		{
			/* Get "shower" */
			prtf(0, hgt - 1, "Show: ");
			(void)askfor_aux(shower, 80);
		}

		/* Hack -- try finding */
		if (k == '/')
		{
			/* Get "finder" */
			prtf(0, hgt - 1, "Find: ");

			if (askfor_aux(finder, 80))
			{
				/* Find it */
				find = finder;
				back = line;
				line = line + 1;

				/* Make finder lowercase */
				for (cnt = 0; finder[cnt] != 0; cnt++)
				{
					finder[cnt] = tolower(finder[cnt]);
				}

				/* Show it */
				strcpy(shower, finder);
			}
		}
		
		/* Hack -- try finding again */
		if (k == '\\')
		{
			if (finder)
			{
				/* Find it */
				find = finder;
				back = 0;
				line = line + 1;

				/* Show it */
				strcpy(shower, finder);
			}
		}

		/* Go to a specific line */
		if (k == '#')
		{
			char tmp[81];
			prtf(0, hgt - 1, "Goto Line: ");
			strcpy(tmp, "0");

			if (askfor_aux(tmp, 80))
			{
				line = atoi(tmp);
			}
		}

		/* Go to a specific file */
		if (k == '%')
		{
			char tmp[81];
			prtf(0, hgt - 1, "Goto File: ");
			strcpy(tmp, "help.hlp");

			if (askfor_aux(tmp, 80))
			{
				if (!show_file(tmp, NULL, 0, mode)) k = ESCAPE;
			}
		}

		/* Go back half a page */
		if (k == '-')
		{
			line = line - (hgt - 4) / 2;
			if (line < 0) line = 0;
		}

		/* Advance half a page */
		if (k == '+')
		{
			line = line + (hgt - 4) / 2;
		}

		/* Advance a single line */
		if ((k == '\n') || (k == '\r'))
		{
			line = line + 1;
		}

		/* Advance one page */
		if (k == ' ')
		{
			line = line + hgt - 4;
		}

		/* Recurse on numbers */
		if (menu)
		{
			int key = -1;

			if (isdigit(k)) key = D2I(k);
			else if (isalpha(k)) key = k - 'A' + 10;

			if ((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if (!show_file(hook[key], NULL, 0, mode))
					k = ESCAPE;
			}
		}

		/* Dump to file */
		if (k == '|')
		{
			FILE *ffp;
			char outfile[1024];
			char xtmp[82];

			/* Start with an empty filename */
			xtmp[0] = '\0';

			/* Get a filename */
			if (!get_string(xtmp, 80, "File name: ")) continue;

			/* Check for a "valid" name */
			if (!(xtmp[0] && (xtmp[0] != ' '))) continue;

			/* Build the filename */
			path_build(outfile, 1024, ANGBAND_DIR_USER, xtmp);

			/* Close the input file */
			my_fclose(fff);

			/* Hack -- Re-Open the input file */
			fff = my_fopen(path, "r");

			/* Open the output file */
			ffp = my_fopen(outfile, "w");

			/* Oops */
			if (!(fff && ffp))
			{
				msgf("Failed to open file.");
				k = ESCAPE;
				break;
			}

			/* Write the file line by line */
			while (!my_raw_fgets(fff, xtmp, 80))
				my_fputs(ffp, xtmp, 80);

			/* Close the files */
			my_fclose(fff);
			my_fclose(ffp);

			/* Hack -- Re-Open the input file */
			fff = my_fopen(path, "r");
		}

		/* Exit on escape */
		if (k == ESCAPE) break;
	}
	
	/* Restore the screen */
	screen_load();

	/* Close the file */
	my_fclose(fff);

	/* Escape */
	if (k == ESCAPE) return (FALSE);

	/* Normal return */
	return (TRUE);
}


/*
 * Peruse the On-Line-Help
 */
void do_cmd_help(void)
{
	/* Peruse the main help file */
	(void)show_file("help.hlp", NULL, 0, 0);
}


/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
void process_player_name(bool sf)
{
	int i, k = 0;


	/* Cannot be too long */
	if (strlen(player_name) > 15)
	{
		/* Name too long */
		quit_fmt("The name '%s' is too long!", player_name);
	}

	/* Cannot contain "icky" characters */
	for (i = 0; player_name[i]; i++)
	{
		/* No control characters */
		if (iscntrl(player_name[i]))
		{
			/* Illegal characters */
			quit_fmt("The name '%s' contains control chars!", player_name);
		}
	}


#ifdef MACINTOSH

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
		char c = player_name[i];

		/* Convert "dot" to "underscore" */
		if (c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
		char c = player_name[i];

		/* Accept some letters */
		if (isalpha(c) || isdigit(c)) player_base[k++] = c;

		/* Convert space, dot, and underscore to underscore */
		else if (strchr(". _", c)) player_base[k++] = '_';
	}

#endif


#if defined(WINDOWS) || defined(MSDOS)

	/* Hack -- max length */
	if (k > 8) k = 8;

#endif

	/* Terminate */
	player_base[k] = '\0';

	/* Require a "base" name */
	if (!player_base[0]) strcpy(player_base, "PLAYER");


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif

	/* Change the savefile name */
	if (sf)
	{
		char temp[128];

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		strnfmt(temp, 128, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		strnfmt(temp, 128, "%s", player_base);
#endif

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		strnfmt(temp, 128, "%s.sv", player_base);
#endif /* VM */

		/* Build the filename */
		path_build(savefile, 1024, ANGBAND_DIR_SAVE, temp);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_player(0)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?  XXX XXX
 */
void change_player_name(void)
{
	char tmp[32];

	/* Clear last line */
	clear_from(22);

	/* Prompt and ask */
	prtf(2, 23, "[Enter your player's name above, or hit ESCAPE]");

	/* Ask until happy */
	while (1)
	{
		/* Go to the "name" field */
		Term_gotoxy(COL_NAME + WID_NAME, 2);

		/* Save the player name */
		strcpy(tmp, player_name);

		/* Get an input, ignore "Escape" */
		if (askfor_aux(tmp, 16)) strcpy(player_name, tmp);

		/* Process the player name */
		process_player_name(FALSE);

		/* All done */
		break;
	}

	/* Re-Draw the name (in light blue) */
	put_fstr(COL_NAME + WID_NAME, 2, CLR_L_BLUE "%-15.15s", player_name);

	/* Erase the prompt, etc */
	clear_from(22);
}

/* Gets a name for the character, reacting to name changes.
 * Taken from V 2.9.0.
 */

void get_character_name(void)
{
	char tmp[16];

	/* Save the player name */
	strcpy(tmp, player_name);

	/* Prompt for a new name */
	if (get_string(tmp, sizeof(tmp), "Enter a name for your character: "))
	{
		/* Use the name */
		strcpy(player_name, tmp);

		/* Process the player name */
		process_player_name(FALSE);
	}
}


/*
 * Hack -- commit suicide
 */
void do_cmd_suicide(void)
{
	int i;

	/* Flush input */
	flush();

	/* Verify Retirement */
	if (p_ptr->state.total_winner)
	{
		/* Verify */
		if (!get_check("Do you want to retire? ")) return;
	}

	/* Verify Suicide */
	else
	{
		/* Verify */
		if (!get_check("Do you really want to commit suicide? ")) return;

		if (!p_ptr->state.noscore)
		{
			/* Special Verification for suicide */
			prtf(0, 0, "Please verify SUICIDE by typing the '@' sign: ");
			flush();
			i = inkey();
			clear_msg();
			if (i != '@') return;
		}
	}

	/* Stop playing */
	p_ptr->state.playing = FALSE;

	/* Kill the player */
	p_ptr->state.is_dead = TRUE;

	/* Leaving */
	p_ptr->state.leaving = TRUE;

	/* Cause of death */
	(void)strcpy(p_ptr->state.died_from, "Quitting");
}


/*
 * Save the game
 */
void do_cmd_save_game(int is_autosave)
{
	/* Autosaves do not disturb */
	if (is_autosave)
	{
		msgf("Autosaving the game...");
	}
	else
	{
		/* Disturb the player */
		disturb(TRUE);
	}

	/* Clear messages */
	message_flush();

	/* Handle stuff */
	handle_stuff();

	/* Message */
	prtf(0, 0, "Saving game...");

	/* Refresh */
	Term_fresh();

	/* The player is not dead */
	(void)strcpy(p_ptr->state.died_from, "(saved)");

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Save the player */
	if (save_player())
	{
		prtf(0, 0, "Saving game... done.");
	}

	/* Save failed (oops) */
	else
	{
		prtf(0, 0, "Saving game... failed!");
	}

	/* Allow suspend again */
	signals_handle_tstp();

	/* Refresh */
	Term_fresh();

	/* Clear messages. */
	message_flush();

	/* Hack -- erase the message line. */
	clear_msg();

	/* Note that the player is not dead */
	(void)strcpy(p_ptr->state.died_from, "(alive and well)");
}


/*
 * Save the game and exit
 */
void do_cmd_save_and_exit(void)
{
	p_ptr->state.playing = FALSE;

	/* Leaving */
	p_ptr->state.leaving = TRUE;
}

/*
 * Center a format string in the buffer.
 *
 * The first parameter on the stack must be the width
 *  to center in.
 *
 * The second must be the string to center with.
 * This is treated as a format string - so may contain
 * other commands etc...
 */
void center_string(char *buf, uint max, cptr fmt, va_list *vp)
{
	int i, j;

	cptr str;
	
	char tmp[1024];
	
    int size;
	
	/* Unused parameter */
	(void)fmt;
	    
    /* Get the size of the string to center in */
	size = va_arg(*vp, int);
	
	/* Get the string to center with. */
	str = va_arg(*vp, cptr);
	
	/* Expand the string */
	vstrnfmt(tmp, 1024, str, vp);
	
	/* Total length */
	i = strlen(tmp);

	/* Necessary border */
	j = (size - i) / 2;

	/* Mega-Hack center the (format) string in the buffer */
	strnfmt(buf, max, "%*s%s%*s", j, "", tmp, size - i - j, "");
}




/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux) (void) = NULL;


/*
 * Display a "tomb-stone"
 */
static void print_tomb(void)
{
	bool done = FALSE;

	/* Do we use a special tombstone ? */
	if (tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux) ();
	}

	/* Print the text-tombstone */
	if (!done)
	{
		char buf[1024];

		FILE *fp;

		time_t ct = time((time_t) 0);


		/* Clear screen */
		Term_clear();

		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_FILE, "dead.txt");

		/* Open the News file */
		fp = my_fopen(buf, "r");

		/* Dump */
		if (fp)
		{
			int i = 0;

			/* Dump the file to the screen */
			while (0 == my_fgets(fp, buf, 1024))
			{
				/* Display and advance */
				put_fstr(0, i++, buf);
			}

			/* Close */
			my_fclose(fp);
		}


		put_fstr(11, 6, "%v", center_string, 31, player_name);

		put_fstr(11, 7, "%v", center_string, 31, "the");

		
		/* King or Queen */
		if (p_ptr->state.total_winner || (p_ptr->lev > PY_MAX_LEVEL))
		{
			put_fstr(11, 8, "%v", center_string, 31, "Magnificent");
		}

		/* Normal */
		else
		{
			put_fstr(11, 8, "%v", center_string, 31,
					 player_title[p_ptr->rp.pclass][(p_ptr->lev - 1) / 5]);
		}

		put_fstr(11, 10, "%v", center_string, 31, cp_ptr->title);

		put_fstr(11, 11, "%v", center_string, 31, "Level: %d", (int)p_ptr->lev);

		put_fstr(11, 12, "%v", center_string, 31, "Exp: %ld", (long)p_ptr->exp);

		put_fstr(11, 13, "%v", center_string, 31, "AU: %ld", (long)p_ptr->au);

		put_fstr(11, 14, "%v", center_string, 31, "Killed on Level %d", p_ptr->depth);


		if (strlen(p_ptr->state.died_from) > 24)
		{
			put_fstr(11, 15, "%v", center_string, 31, "by %.24s.", p_ptr->state.died_from);
		}
		else
		{
			put_fstr(11, 15, "%v", center_string, 31, "by %s.", p_ptr->state.died_from);
		}

		put_fstr(11, 17, "%v", center_string, 31, "%-.24s", ctime(&ct));
	}
}


/*
 * Display some character info
 */
static void show_info(void)
{
	int i, j, l;
	object_type *o_ptr;
	store_type *st_ptr;

	/* Hack -- Know everything in the equipment */
	for (i = 0; i < EQUIP_MAX; i++)
	{
		o_ptr = &p_ptr->equipment[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
		object_mental(o_ptr);

		/* Save all the known flags */
		o_ptr->kn_flags[0] = o_ptr->flags[0];
		o_ptr->kn_flags[1] = o_ptr->flags[1];
		o_ptr->kn_flags[2] = o_ptr->flags[2];
		o_ptr->kn_flags[3] = o_ptr->flags[3];
	}

	/* Hack -- Know everything in the inventory */
	OBJ_ITT_START (p_ptr->inventory, o_ptr)
	{
		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
		object_mental(o_ptr);

		/* Save all the known flags */
		o_ptr->kn_flags[0] = o_ptr->flags[0];
		o_ptr->kn_flags[1] = o_ptr->flags[1];
		o_ptr->kn_flags[2] = o_ptr->flags[2];
		o_ptr->kn_flags[3] = o_ptr->flags[3];
	}
	OBJ_ITT_END;

	for (i = 1; i < z_info->wp_max; i++)
	{
		for (j = 0; j < place[i].numstores; j++)
		{
			st_ptr = &place[i].store[j];

			if (st_ptr->type == BUILD_STORE_HOME)
			{
				/* Hack -- Know everything in the home */
				OBJ_ITT_START (st_ptr->stock, o_ptr)
				{
					/* Aware and Known */
					object_aware(o_ptr);
					object_known(o_ptr);
					object_mental(o_ptr);

					/* Save all the known flags */
					o_ptr->kn_flags[0] = o_ptr->flags[0];
					o_ptr->kn_flags[1] = o_ptr->flags[1];
					o_ptr->kn_flags[2] = o_ptr->flags[2];
					o_ptr->kn_flags[3] = o_ptr->flags[3];
				}
				OBJ_ITT_END;
			}
		}
	}

	/* Hack -- Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Display player */
	display_player(DISPLAY_PLAYER_STANDARD);

	/* Prompt for inventory */
	prtf(0, 23, "Hit any key to see more information (ESC to abort): ");

	/* Flush keys */
	flush();

	/* Allow abort at this point */
	if (inkey() == ESCAPE) return;


	/* Show equipment */
	Term_clear();

	/* Equipment -- if any */
	item_tester_full = TRUE;
	show_equip(FALSE);

	prtf(0, 0, "You are using: -more-");

	/* Flush keys */
	flush();

	if (inkey() == ESCAPE) return;


	/* Show inventory */
	Term_clear();

	/* Inventory -- if any */
	item_tester_full = TRUE;
	show_list(p_ptr->inventory, FALSE);

	prtf(0, 0, "You are carrying: -more-");

	/* Flush keys */
	flush();

	if (inkey() == ESCAPE) return;

	for (i = 1; i < z_info->wp_max; i++)
	{
		for (l = 0; l < place[i].numstores; l++)
		{
			st_ptr = &place[i].store[l];

			if (st_ptr->type == BUILD_STORE_HOME)
			{
				/* Home -- if anything there */
				if (st_ptr->stock)
				{
					/* Initialise counter */
					j = 0;

					/* Clear screen */
					Term_clear();

					/* Display contents of the home */
					OBJ_ITT_START (st_ptr->stock, o_ptr)
					{
						/* Print header, clear line */
						prtf(4, j + 2, "%c) %s%v", I2A(j),
							 color_seq[tval_to_attr[o_ptr->tval]],
							 OBJECT_FMT(o_ptr, TRUE, 3));

						/* Show 12 items at a time */
						if (j == 12)
						{
							/* Caption */
							prtf(0, 0, "Your home in %s: -more-",
								 place[i].name);

							/* Flush keys */
							flush();

							/* Wait for it */
							if (inkey() == ESCAPE) return;

							/* Restart counter */
							j = 0;

							/* Clear screen */
							Term_clear();
						}
					}
					OBJ_ITT_END;
				}
			}
		}
	}
}


static void close_game_handle_death(void)
{
	char ch;

	/* Handle retirement */
	if (p_ptr->state.total_winner)
	{
		/* Save winning message to notes file. */
		if (take_notes)
		{
			add_note_type(NOTE_WINNER);
		}

		kingly();
	}

	/* Save memories */
	if (!munchkin_death || get_check("Save death? "))
	{
		if (!save_player()) msgf("death save failed!");
	}

#if 0
	/* Dump bones file */
	make_bones();
#endif

	/* Inform notes file that you are dead */
	if (take_notes)
	{
		char long_day[30];
		time_t ct = time((time_t *) NULL);

		/* Get the date */
		(void)strftime(long_day, 30, "%Y-%m-%d at %H:%M:%S", localtime(&ct));

		/* Output to the notes file */
		output_note("\n%s was killed by %s on %s\n", player_name,
				p_ptr->state.died_from, long_day);
	}

	/* Enter player in high score list */
	enter_score();

	/* You are dead */
	print_tomb();

	/* Describe options */
	prtf(0, 23, "(D) Dump char record  (C) Show char info  (T) Show top scores  (ESC) Exit");

	/* Flush messages */
	message_flush();

	/* Flush all input keys */
	flush();

	/* Player selection */
	while (TRUE)
	{
		/* Save screen */
		/* Note that Term_save() and Term_load() must match in pairs */
		Term_save();

		/* Flush all input keys */
		flush();

		ch = inkey();

		switch (ch)
		{
			case ESCAPE:
			{
				/* Flush the keys */
				flush();

				if (get_check("Do you really want to exit? "))
				{
					/* Save dead player */
					if (!save_player())
					{
						msgf("Death save failed!");
						message_flush();
					}

#if 0
					/* Dump bones file */
					make_bones();
#endif

					/* XXX We now have an unmatched Term_save() */
					Term_load();

					/* Go home, we're done */
					return;
				}
				else
				{
					break;
				}
			}

			case 'd':
			case 'D':
			{
				/* Dump char file */
				char tmp[160] = "";

				/* Prompt */
				put_fstr(0, 23, "Filename: ");

				/* Ask for filename (or abort) */
				if (!askfor_aux(tmp, 60)) break;

				/* Ignore Return */
				if (!tmp[0]) break;

				/* Dump a character file */
				(void)file_character(tmp, FALSE);

				break;
			}

			case 'c':
			case 'C':
			{
				/* Show char info */
				show_info();
				break;
			}

			case 't':
			case 'T':
			{
				/* Show top twenty */
				top_twenty();
				break;
			}
		}

		/* Restore the screen */
		Term_load();
	}
}


/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 */
void close_game(void)
{
	char buf[1024];

	/* Handle stuff */
	handle_stuff();

	/* Flush the messages */
	message_flush();

	/* Flush the input */
	flush();

	/* No suspending now */
	signals_ignore_tstp();


	/* Hack -- Character is now "icky" */
	character_icky = TRUE;


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_APEX, "scores.raw");

	/* Grab permissions */
	safe_setuid_grab();

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	safe_setuid_drop();

	if (p_ptr->state.is_dead)
	{
		/* Handle death */
		close_game_handle_death();
	}

	/* Still alive */
	else
	{
		int wid, hgt;
	
		/* Save the game */
		do_cmd_save_game(FALSE);

		/* If note-taking enabled, write session end to notes file */
		if (take_notes)
		{
			add_note_type(NOTE_SAVE_GAME);
		}
		
		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Prompt for scores XXX XXX XXX */
		prtf(0, hgt - 1, "Press Return (or Escape).");

		/* Predict score (or ESCAPE) */
		if (inkey() != ESCAPE) predict_score();
	}


	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
	/* If nothing important has happened, just quit */
	if (!character_generated || character_saved) quit("panic");

	/* Mega-Hack -- see "msgf()" */
	msg_flag = FALSE;

	/* Clear the top line */
	clear_msg();

	/* Hack -- turn off some things */
	disturb(TRUE);

	/* Mega-Hack -- Delay death */
	if (p_ptr->chp < 0) p_ptr->state.is_dead = FALSE;

	/* Hardcode panic save */
	p_ptr->state.panic_save = 1;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
	(void)strcpy(p_ptr->state.died_from, "(panic save)");

	/* Panic save, or get worried */
	if (!save_player()) quit("panic save failed!");

	/* Successful panic save */
	quit("panic save succeeded!");
}


/*
 * Get a random line from a file
 * Based on the monster speech patch by Matt Graham,
 */
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE *fp;
	char buf[1024];
	int line, counter, test, numentries;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, file_name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Failed */
	if (!fp) return (-1);

	/* Find the entry of the monster */
	while (TRUE)
	{
		/* Get a line from the file */
		if (my_fgets(fp, buf, 1024) == 0)
		{
			/* Look for lines starting with 'N:' */
			if ((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				if (buf[2] == '*') break;

				/* Get the monster number */
				else if (sscanf(&(buf[2]), "%d", &test) != EOF)
				{
					/* Is it the right monster? */
					if (test == entry) break;
				}
				else
				{
					/* Error while converting the monster number */
					msgf("Error - end of file.");

					my_fclose(fp);
					return (-1);
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return (-1);
		}

	}

	/* Get the number of entries */
	while (TRUE)
	{
		/* Get the line */
		if (my_fgets(fp, buf, 1024) == 0)
		{
			/* Look for the number of entries */
			if (isdigit(buf[0]))
			{
				/* Get the number of entries */
				numentries = atoi(buf);
				break;
			}
		}
		else
		{
			/* Reached end of file without finding the number */
			msgf("Error - end of file.");

			my_fclose(fp);
			return (-1);
		}
	}

	if (numentries > 0)
	{
		/* Grab an appropriate line number */
		line = randint1(numentries);

		/* Get the random line */
		for (counter = 0; counter < line; counter++)
		{
			/* Try to read the line, skipping comments */
			while (TRUE)
			{
				test = my_fgets(fp, buf, 1024);
				if (test != 0 || buf[0] != '#') break;
			}

			if (test != 0)
			{
				/* Error - End of file */
				msgf("Error - end of file.");

				my_fclose(fp);
				return (-1);
			}
		}

		/* Copy the line */
		strcpy(output, buf);
	}
	else
	{
		/* Close the file */
		my_fclose(fp);

		return (-1);
	}

	/* Close the file */
	my_fclose(fp);

	/* Success */
	return (0);
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	/* Redraw the term */
	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if (p_ptr->state.is_dead)
	{
		/* Mark the savefile */
		(void)strcpy(p_ptr->state.died_from, "Abortion");

		/* Close stuff */
		close_game();

		/* Quit */
		quit("interrupt");
	}

	/* Allow suicide (after 5) */
	else if (signal_count >= 5)
	{
		/* Cause of "death" */
		(void)strcpy(p_ptr->state.died_from, "Interrupting");

		/* Stop playing */
		p_ptr->state.playing = FALSE;

		/* Suicide */
		p_ptr->state.is_dead = TRUE;

		/* Leaving */
		p_ptr->state.leaving = TRUE;

		/* Close stuff */
		close_game();

		/* Quit */
		quit("interrupt");
	}

	/* Give warning (after 4) */
	else if (signal_count >= 4)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);

		/* Display the cause */
		prtf(0, 0, "Contemplating suicide!");

		/* Flush */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if (signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);

	/* Give a warning */
	prtf(0, 23, CLR_RED "A gruesome software bug LEAPS out at you!");

	/* Message */
	put_fstr(45, 23, CLR_RED "Panic save...");

	/* Flush output */
	Term_fresh();

	/* Panic Save */
	p_ptr->state.panic_save = 1;

	/* Panic save */
	(void)strcpy(p_ptr->state.died_from, "(panic save)");

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Attempt to save */
	if (save_player())
	{
		put_fstr(45, 23, CLR_RED "Panic save succeeded!");
	}

	/* Save failed */
	else
	{
		put_fstr(45, 23, CLR_RED "Panic save failed!");
	}

	/* Flush output */
	Term_fresh();

	/* Quit */
	quit("software bug");
}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else  /* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}

#endif /* HANDLE_SIGNALS */
