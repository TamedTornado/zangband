project('Zangband', 'c')

# Add a compiler argument to ensure SET_UID is not defined
add_global_arguments('-USET_UID', language: 'c')

# Check if the target system is Windows and set the appropriate linker flags
if host_machine.system() == 'windows'
    # For a GUI application
    add_global_link_arguments('/SUBSYSTEM:WINDOWS', language: 'c')
    # If you need to explicitly set the entry point, you can do so with /ENTRY:WinMainCRTStartup
    # add_global_link_arguments('/ENTRY:WinMainCRTStartup', language: 'c')
endif

subdir('src/lua')
# tolua = executable('tolua', tolua_files) # Create tolua executable


# subdir('src/tk')

subdir('src')
executable('zangband', src_files, tolua_files, c_args: ['-USET_UID','-UUSE_SOUND', '-DWINDOWS'], link_args: ['/SUBSYSTEM:WINDOWS', '/ENTRY:WinMainCRTStartup'])
