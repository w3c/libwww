## Process this file with Automake to create Makefile.in.

SUBDIRS = src User

#	Some extra make directives
inc-recursive:
	for subdir in $(SUBDIRS); do		\
	  target=`echo $@ | sed s/-recursive//`; \
	  echo making $$target in $$subdir;	\
	  (cd $$subdir && $(MAKE) $$target)	\
	   || case "$(MFLAGS)" in *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
inc : inc-recursive
	@echo Include files generated from hypertext.
