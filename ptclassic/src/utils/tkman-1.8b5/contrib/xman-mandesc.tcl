proc manDesc_parseXman {
	{use_MANPATH 1}
	{volkeys     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijkmqrstuvwxyz90"}} {
# copyright: D. Theobald, Forschungszentrum Informatik, Karlsruhe, Germany
# version:   23.12.94

   global env man manx

   set keypos  0
   set self    parseXman

   foreach dir [expr {$use_MANPATH ? [split $env(MANPATH) :] : $manx(paths)}] {
      DEBUG {puts stderr "$self($dir)"}

      if {[file readable $dir/mandesc]} {
         set file [open $dir/mandesc r]
         while {[gets $file line] != -1} {
	    DEBUG {puts stderr "$self($dir)($line)"}

	    if {[set line [string trim $line]] == "no default sections"} {
	       set cmd "manDescDelete * $dir*"
	       DEBUG {puts stderr "$self($cmd)"}; eval $cmd

	    } elseif {$line != ""} {
	       set lkey [string index $line 0]
	       set lvol [string range $line 1 end]

	       if {[info exists volume($lvol,key)]} {
	          set vkey $volume($lvol,key)
	       } else {
	          set vkey [string index $volkeys $keypos]
	          incr keypos
	          set volume($lvol,key) $vkey

	          lappend volume(new) [list $vkey $lvol]
	       }
	       if {[file isdirectory "$dir/man$lkey"]} {
		  lappend voldirs($vkey) "$dir/man$lkey"
	       }
	    }
         }
         close $file
      }
   }
   if {[info exists volume(new)]} {
      set cmd "manDescAddSects {$volume(new)}"
      DEBUG {puts stderr "$self($cmd)"}; eval $cmd
   }
   if {[info exists voldirs]} {
      foreach vkey [array names voldirs] {
	 set cmd [list manDescAdd $vkey $voldirs($vkey)]
	 DEBUG {puts stderr "$self($cmd)"}; eval $cmd
      }
   }
}
