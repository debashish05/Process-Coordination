# Process Coordination

Solution to few problems on process coordination

## Q1

### Running code

* Run `gcc editors_reporters.c -o editors_reporters.out -pthread` to compile
* Run `./editors_reporters.out <no of editors> <no of reporters>` to execute the program
* A log file consisting of accepted reports and rejected reports is generated for each editor

### Editing code

* Main logic followed by each editor is present inside `begin` function
