; Version $Id$
; Author:	T. M. Parks
; Created:	23 November 1993
;
; Support for pt-info.sty

(put 'Author 'latexinfo-format 'latexinfo-format-author)
(put 'Contrib 'latexinfo-format 'latexinfo-format-author)

;
; Command for beginning a description, such as statelist, blocklist.
;

(defvar ptinfo-index-suffix nil)

(defun ptinfo-index-description ()
  (setq ptinfo-index-suffix (latexinfo-parse-line-arg))
  (latexinfo-discard-command)
  (save-excursion (insert "\\begin{description}\n")))

;
; Commands for formatting a statelist environment
;
(put 'statelist 'latexinfo-format 'ptinfo-index-description)
(put 'statelist 'latexinfo-end 'latexinfo-end-description)
(put 'state 'latexinfo-format 'ptinfo-state)

(defun ptinfo-state ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg))
        (default (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "(\\code{" type "}) Default = \\samp{" default "}")
      (insert "\\vindex{" name " " ptinfo-index-suffix "}\n"))))

;
; Commands for formatting a portlist environment
;
(put 'portlist 'latexinfo-format 'ptinfo-index-description)
(put 'portlist 'latexinfo-end 'latexinfo-end-description)
(put 'port 'latexinfo-format 'ptinfo-port)

(defun ptinfo-port ()
  (let ((type (latexinfo-parse-line-arg))
        (name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "(\\code{" type "})")
      (insert "\\vindex{" name " " ptinfo-index-suffix "}\n"))))

;
; Commands for formatting a blocklist environment
;
(put 'blocklist 'latexinfo-format 'ptinfo-index-description)
(put 'blocklist 'latexinfo-end 'latexinfo-end-description)
(put 'block 'latexinfo-format 'ptinfo-block)

(defun ptinfo-block ()
  (let ((name (latexinfo-parse-line-arg)))
    (latexinfo-discard-command)
    (save-excursion
      (insert "\\item[" name "]")
      (insert "\\tindex{" name " " ptinfo-index-suffix "}"))))
