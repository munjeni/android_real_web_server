libworker.la: worker.lo fdqueue.lo pod.lo
	$(MOD_LINK) worker.lo fdqueue.lo pod.lo
DISTCLEAN_TARGETS = modules.mk
static = libworker.la
shared =
