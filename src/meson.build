  src_files = files(
    'z-util.c', 'z-virt.c', 'z-form.c', 'z-rand.c', 'z-term.c',
    'variable.c', 'tables.c', 'util.c', 'cave.c',
    'object1.c', 'object2.c', 'monster1.c', 'monster2.c',
    'xtra1.c', 'xtra2.c', 'spells1.c', 'spells2.c',
    'melee1.c', 'melee2.c', 'save.c', 'files.c', 'fields.c',
    'cmd1.c', 'cmd2.c', 'cmd3.c', 'cmd4.c', 'cmd5.c', 'cmd6.c',
    'store.c', 'birth.c', 'load.c', 'ui.c',
    'wizard1.c', 'wizard2.c', 'grid.c', 'rooms.c', 'streams.c',
    'generate.c', 'dungeon.c', 'init1.c', 'init2.c',
    'main-win.c', 'readdib.c',
    'effects.c', 'quest.c', 'racial.c',
    'artifact.c', 'mutation.c', 'flavor.c', 'spells3.c',
    'mspells1.c', 'mspells2.c', 'scores.c', 'mind.c', 'maid-grf.c',
    'bldg.c', 'obj_kind.c', 'wild1.c', 'wild2.c', 'wild3.c', 'avatar.c', 'notes.c',
    'zborg1.c', 'zborg2.c', 'zborg3.c', 'zborg4.c', 'zborg5.c',
    'zborg6.c', 'zborg7.c', 'zborg8.c', 'zborg9.c',
    'zbmagic1.c', 'zbmagic2.c', 'zbmagic3.c',
    'script.c',
    'l-monst.c', 'l-object.c', 'l-player.c', 'l-random.c', 'l-ui.c', 'l-misc.c', 'l-spell.c'
  )

angband_rc = files('angband.rc')

angband_res = custom_target('resource_file', output: 'angband.res', command: ['rc', '/fo@OUTPUT@', angband_rc ])

