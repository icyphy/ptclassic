; Version $Id$
; Author:	T. M. Parks
; Created:	23 November 1993
;
; Support for pt-info.sty

(put 'Author 'latexinfo-format 'latexinfo-format-author)
(put 'Contrib 'latexinfo-format 'latexinfo-format-author)

;
; Commands for formatting a statelist environment
;
(put 'statelist 'latexinfo-format 'latexinfo-statelist)
(put 'statelist 'latexinfo-end 'latexinfo-end-statelist)
(put 'statelist 'latexinfo-item 'latexinfo-state-item)
(put 'state 'latexinfo-format 'latexinfo-item)

(defun latexinfo-statelist ()
  (latexinfo-begin-list 'statelist ""))

(defun latexinfo-end-statelist ()
  (latexinfo-end-list 'statelist))

(defun latexinfo-state-item ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg))
        (default (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert ?\b name " (\\code{" type "})")
      (insert " Default = \\samp{" default "}\n"))))

;
; Commands for formatting a portlist environment
;
(put 'portlist 'latexinfo-format 'latexinfo-portlist)
(put 'portlist 'latexinfo-end 'latexinfo-end-portlist)
(put 'portlist 'latexinfo-item 'latexinfo-port-item)
(put 'port 'latexinfo-format 'latexinfo-item)

(defun latexinfo-portlist ()
  (latexinfo-begin-list 'portlist ""))

(defun latexinfo-end-portlist ()
  (latexinfo-end-list 'portlist))

(defun latexinfo-port-item ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert ?\b  name " (\\code{" type "})\n"))))

