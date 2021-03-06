# $FreeBSD$

.include <bsd.own.mk>

PROG=clang

SRCDIR=	tools/clang/tools/driver
SRCS=	cc1_main.cpp \
	cc1as_main.cpp \
	driver.cpp

CFLAGS+= -pthread
LDADD+=-lpthread
LDADD+=-ltermlib
LDADD+=-lz

.if ${MK_SHARED_TOOLCHAIN:L} == "no"
LDSTATIC=-static
.endif

LINKS=	${BINDIR}/clang ${BINDIR}/clang++ \
	${BINDIR}/clang ${BINDIR}/clang-cpp
MLINKS=	clang.1 clang++.1 \
	clang.1 clang-cpp.1
.if ${MK_CLANG_IS_CC:L} != "no"
LINKS+=	${BINDIR}/clang ${BINDIR}/cc \
	${BINDIR}/clang ${BINDIR}/c++ \
	${BINDIR}/clang ${BINDIR}/CC
MLINKS+= clang.1 cc.1 \
	clang.1 c++.1 \
	clang.1 CC.1
.endif

TGHDRS=	\
	DiagnosticCommonKinds \
	DiagnosticDriverKinds \
	DiagnosticFrontendKinds \
	DiagnosticLexKinds \
	DiagnosticSemaKinds \
	Options

.if !defined(EARLY_BUILD) && defined(MK_CLANG_FULL) && ${MK_CLANG_FULL:L} != "no"
_clangstaticanalyzer= \
	clangstaticanalyzerfrontend \
	clangstaticanalyzercheckers \
	clangstaticanalyzercore
_clangarcmigrate= \
	clangarcmigrate
.endif # !EARLY_BUILD && MK_CLANG_FULL

LIBDEPS=clangfrontendtool \
	clangrewritefrontend \
	clangfrontend \
	clangrewrite \
	clangdriver \
	clangserialization \
	clangcodegen \
	clangparse \
	clangsema \
	${_clangstaticanalyzer} \
	${_clangarcmigrate} \
	clanganalysis \
	clangedit \
	clangast \
	clangbasic \
	clanglex \
	llvmlinker \
	llvmirreader \
	llvmipo \
	llvmvectorize \
	llvminstrumentation \
	llvmbitwriter \
	llvmbitreader \
	llvmasmparser \
	llvmaarch64disassembler \
	llvmaarch64codegen \
	llvmaarch64asmparser \
	llvmaarch64desc \
	llvmaarch64info \
	llvmaarch64instprinter \
	llvmaarch64utils \
	llvmarmdisassembler \
	llvmarmcodegen \
	llvmarmasmparser \
	llvmarmdesc \
	llvmarminfo \
	llvmarminstprinter \
	llvmmipsdisassembler \
	llvmmipscodegen \
	llvmmipsasmparser \
	llvmmipsdesc \
	llvmmipsinfo \
	llvmmipsinstprinter \
	llvmpowerpccodegen \
	llvmpowerpcdesc \
	llvmpowerpcinfo \
	llvmpowerpcinstprinter \
	llvmx86disassembler \
	llvmx86asmparser \
	llvmx86codegen \
	llvmselectiondag \
	llvmasmprinter \
	llvmmcdisassembler \
	llvmmcparser \
	llvmcodegen \
	llvmobjcarcopts \
	llvmoption \
	llvmscalaropts \
	llvmprofiledata \
	llvminstcombine \
	llvmtransformutils \
	llvmipa \
	llvmanalysis \
	llvmx86desc \
	llvmx86info \
	llvmtarget \
	llvmx86instprinter \
	llvmmc \
	llvmobject \
	llvmx86utils \
	llvmcore \
	llvmsupport

.include "../clang.prog.mk"
