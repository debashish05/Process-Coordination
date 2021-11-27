# Process Coordination

Solution to few problems on process coordination

## Q1

### Considered approaches

#### Approach 1
```
Create a lock for each report.
Accepted reports and reports currently being read are locked.
Editor iterates over locks and runs trywait.
Reports not locked and not rejected by an editor are listed
and one report is randomly chosen from list.
This approach prevents bottleneck being formed at checking report status.
However, multiple editors may end up accepting the same report. (Problem A)
```
#### Approach 2
```
Create a single lock for entire status list.
Editor accquires lock.
Editor lists all available reports (not currently read and not rejected).
Editor chooses a report at random and updates its status to being read.
Editor releases lock.
Editor reads report.
If editor chooses to accept report, editor accquired lock.
Editor updates report status.
Editor releases lock.
This approach causes a huge bottleneck since when one editor is selecting a report,
no other editor can select one. (Problem A)
Also, when one editor is accepting one report, no other editor can access any report. (Problem B)
```
#### Hybrid approach
```
Create a single lock for entire status list.
Create a lock for each report.
Editor accquires lock for entire status list.
Editor lists all available reports (not currently read and not rejected).
Editor chooses a report at random and updates its status to being read.
Editor accquires lock for that report.
Editor releases lock for entire status list.
Editor reads report.
If editor chooses to accept report, report status is updated.
Editor releases lock for that report.
This approach resolves problem A in approach 1 and problem B in approach 2.
Problem A in approach 2 still remains an issue.
```

### Running code

* Run `gcc editors_reporters.c -o editors_reporters.out -pthread` to compile
* Run `./editors_reporters.out <no of editors> <no of reporters>` to execute the program
* A log file consisting of accepted reports and rejected reports is generated for each editor

### Editing code

* Main logic followed by each editor is present inside `begin` function

## Q3

### Running code

* Run `g++ multi_process.cpp -o multi_process.out`
* Run `g++ multi_thread.cpp -o multi_thread.out -pthread`
* Run `g++ vanilla.cpp -o vanilla.out`
