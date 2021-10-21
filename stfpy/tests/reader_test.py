import sys
import time
import stfpy

def profile_reader(trace, reader_type):
    with reader_type(trace) as r:
        start = time.time()
        for x in r:
            pass
        elapsed = time.time() - start
        print(f'{r.__class__.__name__}: {elapsed} s')

if len(sys.argv) != 2:
    print('Usage: reader_test.py <trace>')
    sys.exit(1)

trace = sys.argv[1]
profile_reader(trace, stfpy.STFBranchReader)
profile_reader(trace, stfpy.STFInstReader)
