Nemea Policer
=============

This repository contains detection module for a [Nemea system](https://github.com/CESNET/Nemea).
The Nemea system is a modular system that consists of independent modules for network traffic analysis and anomaly detection. The aim of this project is monitor a stream of network flows in order to detect network policy violations in the local network. The policer is controlled by a specific language which represents pipelining consisting of filtering and aggregation. These operations can be clearly defined and managed within security rules.

# Installation

## Installing Nemea-Framework
Follow the instructions in:
<https://github.com/CESNET/Nemea-Framework>

# Compilation of Policer

classic compile of module with -O3 option:
```
make
```
compile module in developing mode:
```
make MODE=devel
```
compile module for measure performance purpose:
```
make MODE=perf
```

# Usage

```
./policer -i f:data.dump,u:soc -f rules.txt & ./logger -i u:soc -t
```
where 
* -i option is specification of TRAP interface types and their parameters
* input file data.dump is captured traffic in TRAP format
* output socket "soc" is used to sending results to "logger" module
* rules.txt is file with security rules.
* [logger](https://github.com/CESNET/Nemea-Modules/tree/master/logger) is Nemea module to print UniRec records to stdout.

Other useful module for experimets is [logreplay] (https://github.com/CESNET/Nemea-Modules/tree/master/logreplay).

# Rules

Example of rule.

```
branch spam_detection{
 filter: SRC_IP in [198.51.100.0/24] and DST_PORT = 25; 
 grouper: SRC_IP;
 window: type = global, range = 1 seconds;
 aggregator: smtp_count = COUNT_DISTINCT(DST_IP);
 group-filter: smtp_count > 20; 
 selector: SRC_IP, smtp_count;
}
```

More information about window types, see [link](https://nemea.liberouter.org/doc/aggregation/)
