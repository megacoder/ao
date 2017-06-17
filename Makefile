PREFIX	=/opt
BINDIR	=${PREFIX}/bin

all::	ao

check:: ao
	echo This is a test. | ./ao

clean::
	${RM} *.o a.out lint tags core.*

distclean clobber:: clean
	${RM} ao

install:: ao
	install -D -c -s ao ${BINDIR}/ao

uninstall::
	${RM} ${BINDIR}/ao
