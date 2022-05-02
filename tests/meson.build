catch2 = dependency('catch2')
catch2_tests_exe = executable('main', 'main.cpp',
                              link_with : [evaluate_expression_library],
                              include_directories : inc,
                              dependencies : [catch2])
test('evaluate expression catch2 test', catch2_tests_exe)
