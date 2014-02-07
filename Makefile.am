ACLOCAL_AMFLAGS = -I m4
AM_LIBTOOLFLAGS = --quiet

lib_LTLIBRARIES =	libmi.la

libmi_la_SOURCES =	CLICommand.c CLIOutput.c MIArg.c MIBreakpoint.c \
					MIBreakCommand.c MICommand.c MIEnvironmentCommand.c \
					MIEnvironment.c MIError.c MIEvent.c MIExecCommand.c \
					MIFile.c MIFileCommand.c MIFrame.c MIGDBCommand.c MIList.c \
					MIMemory.c MIMemoryCommand.c MIOOBRecord.c MIOutput.c \
					MIParser.c MIPty.c MIResult.c MIResultRecord.c MISession.c \
					MISignalInfo.c MIStackCommand.c MIString.c MITargetCommand.c \
					MIThread.c MIThreadCommand.c MIDisassembly.c MIValue.c \
					MIVar.c MIVarCommand.c
libmi_la_LDFLAGS =	-shared -avoid-version
			
include_HEADERS =	CLIOutput.h MI.h MIArg.h MIBreakpoint.h MICommand.h \
					MIDisassembly.h MIEnvironment.h MIError.h MIEvent.h \
					MIFile.h MIFrame.h MIList.h MIMemory.h MIOOBRecord.h MIOutput.h \
					MIResult.h MIResultRecord.h MISession.h MISignalInfo.h \
					MIString.h MIThread.h MIValue.h MIVar.h

noinst_PROGRAMS =	test_mi

test_mi_SOURCES =	test_mi.c
test_mi_CFLAGS =	$(AM_CFLAGS) -g
test_mi_LDFLAGS = 	-g
test_mi_LDADD =		-lmi
