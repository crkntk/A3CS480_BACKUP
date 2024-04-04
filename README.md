# CS480_Pages
 Sample outputs are given in sample_output.txt
Demand Paging is a virtual memory management mechanism that would only load and map
pages from persistent storage to memory when needed, i.e., when a process tries to access a
page that is not residing in the physical memory (which would result in a page fault). The OS will
then try to find a free (unoccupied) physical frame, load, and map the page to the free physical
frame as part of the page fault handling. When there is no free frame for allocation, a page
replacement algorithm is used to select an existent mapped page frame to evict and bring in the
page being accessed and map it to the freed frame.
A simulation of demand paging using a multi-level page table
with page replacement if necessary.

Run program in make file
run : make
then run: ./pagingwithpr trace.tr 8 8 8

Optional arguments:
-n N Process only the first N memory accesses / references. Processes all
addresses if not present.

-f N Number of available physical frames. If this argument is specified, page
replacements may be simulated. The default is 999999 if not specified,
simulating an infinite number of frames that no page replacement is
necessary.

-b N number of memory accesses between bitstring updates, this is for
simulating the bitstring update interval for NFU with aging, i.e., how
often (every few memory accesses) the bitstring should be updated.
Default is 10 if not specified.

-l mode Log mode is a string that specifies what to be printed to the standard
output:
bitmasks – Write out the bitmasks for each level starting with the
lowest tree level (root node is at level 0), one per line. In this mode,
you do not need to process any addresses. The program prints
bitmasks and exits. (Use the given log_bitmasks function.)
va2pa – Print virtual address translation to physical address for every
address, one address translation per line. (Use the given log_va2pa
function.)

vpns_pfn – For every virtual address, print its virtual page numbers for
each level followed by the frame number, one address per line.
(Use the given log_vpns_pfn function.)
vpn2pfn_pr – For every address, print vpn, pfn, replaced victim’s vpn
and bitstring if page replacement happened, and page hit or miss,
one address translation per line. (Use the given log_mapping
function.)

offset – Print page offsets of virtual addresses, one address offset per
line. (Use the given print_num_inHex function.)

summary – Print summary statistics. This is the default argument if -l is
not specified. (Use the given log_summary function.) Statistics
reported include the page size, number of addresses processed, hit
and miss rates for pagetable walk, number of page replacements,
number of frames allocated, total bytes required for page table
(hint: use sizeof). You should get a roughly accurate estimate of the
total bytes used for the page table including data used in all page
tree levels. Note your calculated number may not match the
number of total bytes in sample_output.txt (should be close
though), as you may not have strictly the same data members in
your structures as in the solution code, which is fine. But you
should be aware that in general, with more paging levels, less total
bytes would normally be used.
