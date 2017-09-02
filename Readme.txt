To compile the source code write the following in terminal.

cd/src
make

It will compile the program.

To run the program:

processor_cache_simulator <input_HEX_file> <input_CFG_file> <output_SVG_file> <output_results_file>

To run the trace cache simulator:

code/sim -bs <block_size> -us <cache_size> -a <associativity> -<wb/wt> -<wa/wna> <input_trace_file>