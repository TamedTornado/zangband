/*
 * Dump race_info, class_info, spell_names, and magic_info as JSON
 * Stubs the types, includes the real tables.c data
 *
 * Compile: gcc -o dump_data dump_data.c
 * Run: ./dump_data        > races.json
 *      ./dump_data c      > classes.json
 *      ./dump_data s      > spells.json
 *      ./dump_data m      > magic.json
 */

#include <stdio.h>

/* Stub types matching Zangband's definitions */
typedef signed short s16b;
typedef unsigned short u16b;
typedef signed int s32b;
typedef unsigned int u32b;
typedef unsigned char byte;
typedef char bool;
typedef const char *cptr;
#define TRUE 1
#define FALSE 0

/* From defines.h */
#define MAX_RACES 31
#define MAX_CLASS 11
#define MAX_REALM 8

/* Stat indices for spell_stat */
#define A_STR 0
#define A_INT 1
#define A_WIS 2
#define A_DEX 3
#define A_CON 4
#define A_CHR 5

/* Spell book tvals - we don't need actual values, just definitions */
#define TV_SORCERY_BOOK 0
#define TV_LIFE_BOOK 0

/* Stub the structs we need - must match src/types.h */
typedef struct player_race {
    const char *title;
    s16b r_adj[6];
    s16b r_dis, r_dev, r_sav, r_stl, r_sns, r_fos, r_thn, r_thb;
    byte r_mhp;
    byte r_exp;
    byte b_age, m_age;
    byte m_b_ht, m_m_ht, m_b_wt, m_m_wt;
    byte f_b_ht, f_m_ht, f_b_wt, f_m_wt;
    byte infra;
    u32b choice;
} player_race;

typedef struct player_class {
    const char *title;
    s16b c_adj[6];
    s16b c_dis, c_dev, c_sav, c_stl, c_sns, c_fos, c_thn, c_thb;
    s16b x_dis, x_dev, x_sav, x_stl, x_sns, x_fos, x_thn, x_thb;
    s16b c_mhp;
    s16b c_exp;
    s16b pet_upkeep_div;
    bool heavy_sense;
} player_class;

typedef struct magic_type {
    byte slevel;  /* Required level to learn */
    byte smana;   /* Mana cost */
    byte sfail;   /* Base failure chance */
    byte sexp;    /* Experience bonus */
} magic_type;

typedef struct player_magic {
    int spell_book;
    int spell_xtra;
    int spell_stat;
    int spell_type;
    int spell_first;
    int spell_weight;
    magic_type info[MAX_REALM][32];
} player_magic;

/* Include ONLY the data from tables.c */
static player_race race_info[MAX_RACES] =
#include "src/tables.c.races"
;

static player_class class_info[MAX_CLASS] =
#include "src/tables.c.classes"
;

static cptr spell_names[7][32] =
#include "src/tables.c.spells"
;

static player_magic magic_info[MAX_CLASS] =
#include "src/tables.c.magic"
;

static const char *realm_names[7] = {
    "life", "sorcery", "nature", "chaos", "death", "trump", "arcane"
};

static const char *class_names[MAX_CLASS] = {
    "warrior", "mage", "priest", "rogue", "ranger",
    "paladin", "warrior_mage", "chaos_warrior", "monk", "mindcrafter", "high_mage"
};

static const char *stat_names[6] = {
    "str", "int", "wis", "dex", "con", "chr"
};

void print_slug(const char *name) {
    while (*name) {
        if (*name == '-') putchar('_');
        else if (*name >= 'A' && *name <= 'Z') putchar(*name + 32);
        else if (*name == ' ') putchar('_');
        else if (*name == '&') { putchar('_'); putchar('_'); }
        else if (*name == '/') putchar('_');
        else putchar(*name);
        name++;
    }
}

void dump_races(void) {
    int i;
    printf("{\n");
    for (i = 0; i < MAX_RACES; i++) {
        player_race *r = &race_info[i];
        if (!r->title || !r->title[0]) continue;
        if (i > 0) printf(",\n");
        printf("  \""); print_slug(r->title); printf("\": {\n");
        printf("    \"index\": %d,\n", i);
        printf("    \"name\": \"%s\",\n", r->title);
        printf("    \"stats\": { \"str\": %d, \"int\": %d, \"wis\": %d, \"dex\": %d, \"con\": %d, \"chr\": %d },\n",
            r->r_adj[0], r->r_adj[1], r->r_adj[2], r->r_adj[3], r->r_adj[4], r->r_adj[5]);
        printf("    \"skills\": { \"disarm\": %d, \"device\": %d, \"save\": %d, \"stealth\": %d, \"search\": %d, \"searchFreq\": %d, \"melee\": %d, \"ranged\": %d },\n",
            r->r_dis, r->r_dev, r->r_sav, r->r_stl, r->r_sns, r->r_fos, r->r_thn, r->r_thb);
        printf("    \"hitDie\": %d, \"expMod\": %d,\n", r->r_mhp, r->r_exp);
        printf("    \"age\": { \"base\": %d, \"mod\": %d },\n", r->b_age, r->m_age);
        printf("    \"male\": { \"height\": { \"base\": %d, \"mod\": %d }, \"weight\": { \"base\": %d, \"mod\": %d } },\n",
            r->m_b_ht, r->m_m_ht, r->m_b_wt, r->m_m_wt);
        printf("    \"female\": { \"height\": { \"base\": %d, \"mod\": %d }, \"weight\": { \"base\": %d, \"mod\": %d } },\n",
            r->f_b_ht, r->f_m_ht, r->f_b_wt, r->f_m_wt);
        printf("    \"infravision\": %d,\n", r->infra);
        printf("    \"classChoice\": %u\n", (unsigned)r->choice);
        printf("  }");
    }
    printf("\n}\n");
}

void dump_classes(void) {
    int i;
    printf("{\n");
    for (i = 0; i < MAX_CLASS; i++) {
        player_class *c = &class_info[i];
        if (!c->title || !c->title[0]) continue;
        if (i > 0) printf(",\n");
        printf("  \""); print_slug(c->title); printf("\": {\n");
        printf("    \"index\": %d,\n", i);
        printf("    \"name\": \"%s\",\n", c->title);
        printf("    \"stats\": { \"str\": %d, \"int\": %d, \"wis\": %d, \"dex\": %d, \"con\": %d, \"chr\": %d },\n",
            c->c_adj[0], c->c_adj[1], c->c_adj[2], c->c_adj[3], c->c_adj[4], c->c_adj[5]);
        printf("    \"skills\": { \"disarm\": %d, \"device\": %d, \"save\": %d, \"stealth\": %d, \"search\": %d, \"searchFreq\": %d, \"melee\": %d, \"ranged\": %d },\n",
            c->c_dis, c->c_dev, c->c_sav, c->c_stl, c->c_sns, c->c_fos, c->c_thn, c->c_thb);
        printf("    \"xSkills\": { \"disarm\": %d, \"device\": %d, \"save\": %d, \"stealth\": %d, \"search\": %d, \"searchFreq\": %d, \"melee\": %d, \"ranged\": %d },\n",
            c->x_dis, c->x_dev, c->x_sav, c->x_stl, c->x_sns, c->x_fos, c->x_thn, c->x_thb);
        printf("    \"hitDie\": %d, \"expMod\": %d,\n", c->c_mhp, c->c_exp);
        printf("    \"petUpkeepDiv\": %d,\n", c->pet_upkeep_div);
        printf("    \"heavySense\": %s\n", c->heavy_sense ? "true" : "false");
        printf("  }");
    }
    printf("\n}\n");
}

void dump_spells(void) {
    int realm, spell, cls;
    printf("{\n");
    for (realm = 0; realm < 7; realm++) {
        if (realm > 0) printf(",\n");
        printf("  \"%s\": [\n", realm_names[realm]);
        for (spell = 0; spell < 32; spell++) {
            const char *name = spell_names[realm][spell];
            if (spell > 0) printf(",\n");
            printf("    {\n");
            printf("      \"index\": %d,\n", spell);
            printf("      \"name\": \"%s\",\n", name ? name : "");
            printf("      \"classes\": {\n");
            for (cls = 0; cls < MAX_CLASS; cls++) {
                magic_type *s = &magic_info[cls].info[realm][spell];
                if (cls > 0) printf(",\n");
                printf("        \"%s\": { \"level\": %d, \"mana\": %d, \"fail\": %d, \"exp\": %d }",
                    class_names[cls], s->slevel, s->smana, s->sfail, s->sexp);
            }
            printf("\n      }\n");
            printf("    }");
        }
        printf("\n  ]");
    }
    printf("\n}\n");
}

void dump_magic(void) {
    int cls, realm, spell;
    printf("{\n");
    for (cls = 0; cls < MAX_CLASS; cls++) {
        player_magic *m = &magic_info[cls];
        if (cls > 0) printf(",\n");
        printf("  \"%s\": {\n", class_names[cls]);
        printf("    \"spellStat\": \"%s\",\n", stat_names[m->spell_stat]);
        printf("    \"spellFirst\": %d,\n", m->spell_first);
        printf("    \"spellWeight\": %d,\n", m->spell_weight);
        printf("    \"realms\": {\n");
        for (realm = 0; realm < 7; realm++) {
            if (realm > 0) printf(",\n");
            printf("      \"%s\": [\n", realm_names[realm]);
            for (spell = 0; spell < 32; spell++) {
                magic_type *s = &m->info[realm][spell];
                if (spell > 0) printf(",\n");
                printf("        [%d, %d, %d, %d]", s->slevel, s->smana, s->sfail, s->sexp);
            }
            printf("\n      ]");
        }
        printf("\n    }\n");
        printf("  }");
    }
    printf("\n}\n");
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (argv[1][0] == 'c') dump_classes();
        else if (argv[1][0] == 's') dump_spells();
        else if (argv[1][0] == 'm') dump_magic();
        else dump_races();
    } else {
        dump_races();
    }
    return 0;
}
