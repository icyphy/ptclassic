; Version $Id$
; Author:	T. M. Parks
; Created:	20 January 1994
; 
; Extensions to latexinfo.el

; Update command for use in batch mode.
(defun latexinfo-update-everything ()
  (latexinfo-multiple-files-update (buffer-name) t t))

; Return a string which contains the command.
(defun latexinfo-command ()
  (buffer-substring (1+ latexinfo-command-start) latexinfo-command-end) )

; Insert the command without the leading "\".
(defun latexinfo-insert-command ()
  (insert (latexinfo-command))
  (latexinfo-delete-command) )

; Actions for the beginning of a list environment.
(defun latexinfo-begin-list (check arg)
  (latexinfo-push-stack check arg)
  (setq fill-column (- fill-column 5))
  (latexinfo-discard-line))

; Actions for the end of a list environment.
(defun latexinfo-end-list (check)
  (setq fill-column (+ fill-column 5))
  (latexinfo-discard-command)
  (let ((stacktop (latexinfo-pop-stack check)))
     (latexinfo-do-itemize (nth 1 stacktop))))

; Process special index entries just as normal ones.
(put 'cpindexbold 'latexinfo-format 'latexinfo-format-cindex)
(put 'pgindexbold 'latexinfo-format 'latexinfo-format-pindex)
(put 'fnindexbold 'latexinfo-format 'latexinfo-format-findex)
(put 'vrindexbold 'latexinfo-format 'latexinfo-format-vindex)
(put 'tpindexbold 'latexinfo-format 'latexinfo-format-tindex)
(put 'kyindexbold 'latexinfo-format 'latexinfo-format-kindex)

; Commands that should be ignored.
(put 'newcommand 'latexinfo-format 'latexinfo-discard-line-with-args)
(put 'renewcommand 'latexinfo-format 'latexinfo-discard-line-with-args)
(put 'sloppypar 'latexinfo-format 'latexinfo-discard-line)
(put 'sloppypar 'latexinfo-end 'latexinfo-discard-line)
