; Version $Id$
; Author:	T. M. Parks
; Created:	1 December 1993

; Extend the load path.
(setq load-path
  (cons (substitute-in-file-name "$PTOLEMY/doc/elisp") load-path))

; Allow editing the buffer of a read-only file.
(defun disable-read-only ()
; (setq inhibit-read-only t) ; requires emacs-19
  (setq text-mode-hook '(lambda () (setq buffer-read-only nil))))
