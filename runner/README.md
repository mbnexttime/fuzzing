1. собираем modified_test.bc из пакета pass
2. cmake ./
3. make
4. ./runner <mangloed имя функциии> <cpp port> <return arg> (./runner _Z3sumii 6004 int32)
5. java -jar fuzzing.jar (java -jar fuzzing.jar --portCpp 6004 --functionDescriptionPath description.txt)
   Usage: command line parser options_list
    Options: 
    --portCpp, -port_cpp -> port to use to communicate with c++ runner (always required) { Int }
    --functionDescriptionPath, -function_params_path -> path to function params description (always required) { String }
    --thresholdBeforeStop, -threshold_before_stop [1000] -> number of executions before stop in case of not founding new ways { Int }
    --help, -h -> Usage info 

