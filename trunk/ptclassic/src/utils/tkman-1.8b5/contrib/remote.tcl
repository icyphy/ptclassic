#
# Here's how one might invoke TkMan in another application
#
# 1.  include the following proc in the application's Tcl script
# 2.  invoke the tkman proc with the name of the man page to show,
#     for instance:
#         tkman ls
#         tkman cc(1)
#         tkman a.out.5
#

proc TkMan {man} {

   if {[set found [lsearch [winfo interps] tkman*]]==-1} {
      # if TkMan doesn't already exist, start one up
      if {[catch {exec tkman &}]} {puts stdout "tkman not found"; return}

      # wait for it to be registered
      for {set found -1} {$found==-1} {after 200} {
         set found [lsearch [winfo interps] tkman*]
      }

      # wait for it to initialize
      for {set ready 0} {!$ready} {after 200} {
         catch {if {[send tkman set manx(init)]=="1"} {set ready 1}}
      }
   }
   set tkman [lindex [winfo interps] $found]

   # .man is the main window, guaranteed to exist
   set w .man
   send $tkman raise $w
   send $tkman manShowMan $man

   return
}
