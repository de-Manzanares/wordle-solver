add_executable(wordle-solver)

target_sources(
        wordle-solver
        PRIVATE
        cli/cli.cpp
)
target_link_libraries(
        wordle-solver
        PRIVATE
        common-options
        wordle_solver::lib
)
