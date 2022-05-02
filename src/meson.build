evaluate_expression_library = library('evaluate_expression', 'solution.cpp',
                                      link_with : [],
                                      include_directories : inc)

main_exe = executable('main', 'main.cpp',
                      link_with : [evaluate_expression_library],
                      include_directories : inc)

test('main evaluate expression test', main_exe)
