# ----------------------------------------------------------------- #
#           The HMM-Based Speech Synthesis System (HTS)             #
#           developed by HTS Working Group                          #
#           http://hts.sp.nitech.ac.jp/                             #
# ----------------------------------------------------------------- #
#                                                                   #
#  Copyright (c) 2001-2008  Nagoya Institute of Technology          #
#                           Department of Computer Science          #
#                                                                   #
#                2001-2008  Tokyo Institute of Technology           #
#                           Interdisciplinary Graduate School of    #
#                           Science and Engineering                 #
#                                                                   #
# All rights reserved.                                              #
#                                                                   #
# Redistribution and use in source and binary forms, with or        #
# without modification, are permitted provided that the following   #
# conditions are met:                                               #
#                                                                   #
# - Redistributions of source code must retain the above copyright  #
#   notice, this list of conditions and the following disclaimer.   #
# - Redistributions in binary form must reproduce the above         #
#   copyright notice, this list of conditions and the following     #
#   disclaimer in the documentation and/or other materials provided #
#   with the distribution.                                          #
# - Neither the name of the HTS working group nor the names of its  #
#   contributors may be used to endorse or promote products derived #
#   from this software without specific prior written permission.   #
#                                                                   #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            #
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       #
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          #
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          #
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS #
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          #
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     #
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON #
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   #
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    #
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           #
# POSSIBILITY OF SUCH DAMAGE.                                       #
# ----------------------------------------------------------------- #

# setting
SPEAKER = slt
DATASET = cmu_us_arctic

# awk and perl
AWK   = gawk
PERL  = /usr/bin/perl

# SPTK commands
SWAB    = /usr/local/bin/swab
X2X     = /usr/local/bin/x2x
MGCEP   = /usr/local/bin/mcep
LPC2LSP = /usr/local/bin/lpc2lsp
BCP     = /usr/local/bin/bcp
AVERAGE = /usr/local/bin/average
MERGE   = /usr/local/bin/merge
VSTAT   = /usr/local/bin/vstat
NAN     = /usr/local/bin/nan

# MATLAB and STRAIGHT
MATLAB   = : -nodisplay -nosplash -nojvm
STRAIGHT = 

# dumpfeats to extract utterance information
DUMPFEATS = :

# SOX to convert raw audio to RIFF wav
SOX = /usr/bin/sox

# speech analysis conditions
BYTESWAP    = 0     # Little Endian (i*86, x86_64, Alpha, etc.) => 0  Big Endian (PowerPC, SPARC, 680x0, etc.) => 1
SAMPFREQ    = 16000 # Sampling frequency (16kHz)
FRAMESHIFT  = 80    # Frame shift in point (80 = 16000 * 0.005)
FREQWARP    = 0.42  # frequency warping factor
GAMMA       = 0     # pole/zero weight for mel-generalized cepstral (MGC) analysis
MGCORDER    = 39    # order of MGC analysis
MGCLSP      = 0     # convert MGC coefficients into MGC-LSP form 
LNGAIN      = 1     # use log gain rather than linear gain
LOWERF0     = 80    # lower limit for f0 extraction (Hz) 
UPPERF0     = 350   # upper limit for f0 extraction (Hz) 

# windows for calculating delta features
MGCWIN      = win/mgc.win
LF0WIN      = win/lf0.win
BAPWIN      = win/bap.win
NMGCWIN     = 3
NLF0WIN     = 3
NBAPWIN     = 3

all: analysis labels

analysis: straight mgc lf0 bap cmp gv

# labels: label mlf list scp
labels: mlf list scp

straight:
	# Extracting f0, aperiodicity, and spectra using STRAIGHT
	mkdir -p wav sp ap f0
	frameshift=`expr 1000 \* $(FRAMESHIFT) / $(SAMPFREQ)`; \
	echo "path(path,'$(STRAIGHT)');"                      >  scripts/extract.m; \
	echo "prm.F0frameUpdateInterval=$${frameshift};"      >> scripts/extract.m; \
	echo "prm.F0searchUpperBound=$(UPPERF0);"             >> scripts/extract.m; \
	echo "prm.F0searchLowerBound=$(LOWERF0);"             >> scripts/extract.m; \
	echo "prm.spectralUpdateInterval=$${frameshift};"     >> scripts/extract.m; \
	echo ""                                               >> scripts/extract.m
	for raw in raw/$(DATASET)_$(SPEAKER)_*.raw; do \
		base=`basename $${raw} .raw`; \
		wav=wav/$${base}.wav; \
		f0=f0/$${base}.f0; \
		ap=ap/$${base}.ap; \
		sp=sp/$${base}.sp; \
		echo -n "Converting $${raw} to $${wav} and generating MATLAB script..."; \
		$(SOX) -c 1 -s -w -t raw -r $(SAMPFREQ) $${raw} -c 1 -s -w -t wav -r $(SAMPFREQ) $${wav}; \
		echo "fprintf(1,'Processing $${wav}\n');"     >> scripts/extract.m; \
		echo "[x,fs]=wavread('$${wav}');"             >> scripts/extract.m; \
		echo "[f0, ap] = exstraightsource(x,fs,prm);" >> scripts/extract.m; \
		echo "[sp] = exstraightspec(x, f0, fs, prm);" >> scripts/extract.m; \
		echo "ap = ap';"                              >> scripts/extract.m; \
		echo "sp = sp';"                              >> scripts/extract.m; \
		echo "save '$${f0}' f0 -ascii;"               >> scripts/extract.m; \
		echo "save '$${ap}' ap -ascii;"               >> scripts/extract.m; \
		echo "save '$${sp}' sp -ascii;"               >> scripts/extract.m; \
		echo ""                                       >> scripts/extract.m; \
		echo "done"; \
	done; \
	echo "quit;"                                          >> scripts/extract.m
	$(MATLAB) < scripts/extract.m
	rm -f scripts/extract.m

mgc:
	# Extracting MGC or MGC-LSP coefficients from STRAIGHT spectra
	mkdir -p mgc
	for sp in sp/$(DATASET)_$(SPEAKER)_*.sp; do \
		base=`basename $${sp} .sp`; \
		if [ -s $${sp} ]; then \
			if [ $(MGCLSP) -eq 0 ]; then \
				echo "Extracting MGC coefficients from $${sp}"; \
				if [ $(GAMMA) -ne 0 ]; then \
					GAMMAOPT="-g $(GAMMA)"; \
				fi; \
				$(X2X) +af $${sp} | \
				$(MGCEP) -a $(FREQWARP) $${GAMMAOPT} -m $(MGCORDER) -l 1024 -j 0 -f 0.0 -q 3 > mgc/$${base}.mgc; \
			else \
				echo "Extracting MGC-LSP coefficients from $${sp}"; \
				SAMPKHZ=`expr $(SAMPFREQ) / 1000`; \
				if [ $(LNGAIN) -eq 1 ]; then \
					GAINOPT="-l"; \
				fi; \
				$(X2X) +af $${sp} | \
				$(MGCEP) -a $(FREQWARP) -g $(GAMMA) -m $(MGCORDER) -l 1024 -f 0.0 -q 3 -o 4 | \
				$(LPC2LSP) -m $(MGCORDER) -s $${SAMPKHZ} $${GAINOPT} -n 1024 -p 8 -d 1e-8 > mgc/$${base}.mgc; \
			fi; \
			if [ -n "`$(NAN) mgc/$${base}.mgc`" ]; then \
				echo " Failed to extract MGC coefficients from $${sp}"; \
				rm -f mgc/$${base}.mgc; \
			fi; \
		fi; \
	done
	rm -f tmp

lf0:
	# Converting f0 to log f0
	mkdir -p lf0
	for f0 in f0/$(DATASET)_$(SPEAKER)_*.f0; do \
		base=`basename $${f0} .f0`; \
		if [ -s $${f0} ]; then \
			echo "Converting F0 file $${f0} to log F0 file lf0/$${base}.lf0"; \
			$(X2X) +af $${f0} > tmp; \
			$(PERL) scripts/freq2lfreq.pl tmp > lf0/$${base}.lf0; \
			if [ -n "`$(NAN) lf0/$${base}.lf0`" ]; then \
				echo " Failed to extract $${f0}"; \
				rm -f lf0/$${base}.lf0; \
			fi; \
		fi; \
	done
	rm -f tmp

bap:
	# Converting aperiodicity to band aperiodicity
	mkdir -p bap
	for ap in ap/$(DATASET)_$(SPEAKER)_*.ap; do \
		base=`basename $${ap} .ap`; \
		if [ -s $${ap} ]; then \
			echo "Converting aperiodicity file $${ap} to band aperiodicity file bap/$${base}.bap"; \
			$(X2X) +af $${ap} | $(BCP) +f -n 512 -L  64 -s   0 -e  63 -S 0 | $(AVERAGE) -l  64 > bap1; \
			$(X2X) +af $${ap} | $(BCP) +f -n 512 -L  64 -s  64 -e 127 -S 0 | $(AVERAGE) -l  64 > bap2; \
			$(X2X) +af $${ap} | $(BCP) +f -n 512 -L 128 -s 128 -e 255 -S 0 | $(AVERAGE) -l 128 > bap3; \
			$(X2X) +af $${ap} | $(BCP) +f -n 512 -L 128 -s 256 -e 383 -S 0 | $(AVERAGE) -l 128 > bap4; \
			$(X2X) +af $${ap} | $(BCP) +f -n 512 -L 129 -s 384 -e 512 -S 0 | $(AVERAGE) -l 129 > bap5; \
			$(MERGE) -s 0 -l 1 -L 1 bap1 bap2 | \
			$(MERGE) -s 2 -l 2 -L 1 bap3 | \
			$(MERGE) -s 3 -l 3 -L 1 bap4 | \
			$(MERGE) -s 4 -l 4 -L 1 bap5 > bap/$${base}.bap; \
			if [ -n "`$(NAN) bap/$${base}.bap`" ]; then \
				echo " Failed to extract aperiodicity coefficients $${ap}"; \
				rm -f bap/$${base}.bap; \
			fi; \
		fi; \
	done
	rm -f bap{1,2,3,4,5}

cmp: 
	# Composing training data files from mgc and lf0 files
	mkdir -p cmp
	for j in 1; do \
		MGCDIM=`expr $(MGCORDER) + 1`; \
		LF0DIM=1; \
		BAPDIM=5; \
		MGCWINDIM=`expr $(NMGCWIN) \* $${MGCDIM}`; \
		LF0WINDIM=`expr $(NLF0WIN) \* $${LF0DIM}`; \
		BAPWINDIM=`expr $(NBAPWIN) \* $${BAPDIM}`; \
		MGCLF0WINDIM=`expr $${MGCWINDIM} + $${LF0WINDIM}`; \
		BYTEPERFRAME=`expr 4 \* \( $${MGCWINDIM} + $${LF0WINDIM} + $${BAPWINDIM} \)`; \
		for mgc in mgc/$(DATASET)_$(SPEAKER)_*.mgc; do \
			base=`basename $${mgc} .mgc`; \
			MGCWINS=""; \
			LF0WINS=""; \
			BAPWINS=""; \
			if [ -s $${mgc} -a -s lf0/$${base}.lf0 -a -s bap/$${base}.bap ]; then \
				i=1; \
				echo "Composing training data for $${base}"; \
				while [ $${i} -le $(NMGCWIN) ]; do \
					eval MGCWINS=\"$${MGCWINS} $(MGCWIN)$${i}\"; \
					i=`expr $${i} + 1`; \
				done; \
				$(PERL) scripts/window.pl $${MGCDIM} $${mgc} $${MGCWINS} > cmp/tmp.mgc; \
				i=1; \
				while [ $${i} -le $(NLF0WIN) ]; do \
					eval LF0WINS=\"$${LF0WINS} $(LF0WIN)$${i}\"; \
					i=`expr $${i} + 1`; \
				done; \
				$(PERL) scripts/window.pl $${LF0DIM} lf0/$${base}.lf0 $${LF0WINS} > cmp/tmp.lf0; \
				i=1; \
				while [ $${i} -le $(NBAPWIN) ]; do \
					eval BAPWINS=\"$${BAPWINS} $(BAPWIN)$${i}\"; \
					i=`expr $${i} + 1`; \
				done; \
				$(PERL) scripts/window.pl $${BAPDIM} bap/$${base}.bap $${BAPWINS} > cmp/tmp.bap; \
				$(MERGE) +f -s 0 -l $${LF0WINDIM} -L $${MGCWINDIM}     cmp/tmp.mgc     < cmp/tmp.lf0 > cmp/tmp.mgc+lf0; \
				$(MERGE) +f -s 0 -l $${BAPWINDIM} -L $${MGCLF0WINDIM}  cmp/tmp.mgc+lf0 < cmp/tmp.bap > cmp/tmp.cmp; \
				$(PERL) scripts/addhtkheader.pl $(SAMPFREQ) $(FRAMESHIFT) $${BYTEPERFRAME} 9 cmp/tmp.cmp > cmp/$${base}.cmp; \
			fi; \
		done; \
	done
	rm -f cmp/tmp*

gv:
	mkdir -p gv
	rm -f gvdata_mgc gvdata_lf0 gvdata_bap
	# generating GV data
	for raw in raw/$(DATASET)_$(SPEAKER)_*.raw; do \
		base=`basename $${raw} .raw`; \
		if [ -s mgc/$${base}.mgc ]; then \
			$(VSTAT) -n $(MGCORDER) -o 2 -d mgc/$${base}.mgc >> gvdata_mgc; \
		fi; \
		if [ -s lf0/$${base}.lf0 ]; then \
			$(X2X) +fa lf0/$${base}.lf0 | grep -v '\-1e+10' | $(X2X) +af | $(VSTAT) -l 1 -o 2 -d >> gvdata_lf0; \
		fi; \
		if [ -s bap/$${base}.bap ]; then \
			$(VSTAT) -l 5 -o 2 -d bap/$${base}.bap >> gvdata_bap; \
		fi; \
	done
	# calculating GV pdfs
	$(VSTAT) -n $(MGCORDER) -o 0 -d gvdata_mgc > gv/gv-mgc.pdf
	$(VSTAT) -l 1 -o 0 -d gvdata_lf0 > gv/gv-lf0.pdf
	$(VSTAT) -l 5 -o 0 -d gvdata_bap > gv/gv-bap.pdf
	
	# GV pdfs in big endian (for hts_engine)
	# Add header (dimensionality and # of distributions)
	if [ $(BYTESWAP) -eq 1 ]; then \
		MGCDIM=`expr $(MGCORDER) + 1`; \
		LF0DIM=1; \
		BAPDIM=5; \
		echo $${MGCDIM} | $(X2X) +ai >  gv/gv-mgc.pdf.big; \
		echo "1"        | $(X2X) +ai >> gv/gv-mgc.pdf.big; \
		cat  gv/gv-mgc.pdf           >> gv/gv-mgc.pdf.big; \
		echo $${LF0DIM} | $(X2X) +ai >  gv/gv-lf0.pdf.big; \
		echo "1"        | $(X2X) +ai >> gv/gv-lf0.pdf.big; \
		cat  gv/gv-lf0.pdf           >> gv/gv-lf0.pdf.big; \
		echo $${BAPDIM} | $(X2X) +ai >  gv/gv-bap.pdf.big; \
		echo "1"        | $(X2X) +ai >> gv/gv-bap.pdf.big; \
		cat  gv/gv-bap.pdf           >> gv/gv-bap.pdf.big; \
	else \
		MGCDIM=`expr $(MGCORDER) + 1`; \
		LF0DIM=1; \
		BAPDIM=5; \
		echo $${MGCDIM} | $(X2X) +ai | $(SWAB) +f >  gv/gv-mgc.pdf.big; \
		echo "1"        | $(X2X) +ai | $(SWAB) +f >> gv/gv-mgc.pdf.big; \
		$(SWAB) +f gv/gv-mgc.pdf                  >> gv/gv-mgc.pdf.big; \
		echo $${LF0DIM} | $(X2X) +ai | $(SWAB) +f >  gv/gv-lf0.pdf.big; \
		echo "1"        | $(X2X) +ai | $(SWAB) +f >> gv/gv-lf0.pdf.big; \
		$(SWAB) +f gv/gv-lf0.pdf                  >> gv/gv-lf0.pdf.big; \
		echo $${BAPDIM} | $(X2X) +ai | $(SWAB) +f >  gv/gv-bap.pdf.big; \
		echo "1"        | $(X2X) +ai | $(SWAB) +f >> gv/gv-bap.pdf.big; \
		$(SWAB) +f gv/gv-bap.pdf                  >> gv/gv-bap.pdf.big; \
	fi; \
	rm -f gvdata_mgc gvdata_lf0 gvdata_bap

label:
	mkdir -p labels/mono
	mkdir -p labels/full
	# Extracting monophone and fullcontext labels from utterance files
	for utt in utts/$(DATASET)_$(SPEAKER)_*.utt; do \
		base=`basename $${utt} .utt`; \
		if [ -s $${utt} ]; then \
			$(DUMPFEATS)    -eval     scripts/extra_feats.scm \
					-relation Segment \
					-feats    scripts/label.feats \
					-output   tmp \
					$${utt}; \
		fi; \
		$(AWK) -f scripts/label-full.awk tmp > labels/full/$${base}.lab; \
		$(AWK) -f scripts/label-mono.awk tmp > labels/mono/$${base}.lab; \
	done
	rm -f tmp

mlf:
	# Generating monophone and fullcontext Master Label Files (MLF)
	echo "#!MLF!#" > labels/mono.mlf
	echo "\"*/*.lab\" -> \"path_mod/data/labels/mono\"" >> labels/mono.mlf
	echo "#!MLF!#" > labels/full.mlf
	echo "\"*/*.lab\" -> \"path_mod/data/labels/full\"" >> labels/full.mlf

list:
	mkdir -p lists
	# Generating a fullcntext model list occurred in the training data
	rm -f tmp
	for lab in labels/full/*.lab; do \
		if [ -s $${lab} -a -s labels/mono/`basename $${lab}` -a -s cmp/`basename $${lab} .lab`.cmp ]; then \
			sed -e "s/.* //g" $${lab} >> tmp; \
		fi \
	done
	sort -u tmp > lists/full.list
	rm -f tmp
	# Generating a fullcontext model list including unseen models 
	rm -f tmp
	cat lists/full.list > tmp
	for lab in labels/gen/*.lab; do \
		sed -e "s/.* //g" $${lab} >> tmp; \
	done
	sort -u tmp > lists/full_all.list
	rm -f tmp
	# Generating a monophone list 
	rm -f tmp
	for lab in labels/mono/*.lab; do \
		if [ -s $${lab} -a -s labels/full/`basename $${lab}` -a -s cmp/`basename $${lab} .lab`.cmp ]; then \
			sed -e "s/.* //g" $${lab} >> tmp; \
		fi \
	done
	sort -u tmp > lists/mono.list
	rm -f tmp

scp:
	mkdir -p scp
	# Generating a trainig data script
	rm -f scp/train.scp
	for cmp in path_mod/data/cmp/*.cmp; do \
		if [ -s $${cmp} -a -s labels/mono/`basename $${cmp} .cmp`.lab -a -s labels/full/`basename $${cmp} .cmp`.lab ]; then \
			echo $${cmp} >> scp/train.scp; \
		fi \
	done
	# Generating a generation label script
	rm -f scp/gen.scp
	for lab in path_mod/data/labels/gen/*.lab; do \
		echo $${lab} >> scp/gen.scp; \
	done


clean: clean-straight clean-mgc clean-lf0 clean-cmp clean-gv clean-label clean-mlf clean-list clean-scp

clean-straight: clean-sp clean-f0 clean-ap

clean-sp:
	rm -rf sp

clean-f0:
	rm -rf f0

clean-ap:
	rm -rf ap
	
clean-mgc:
	rm -rf mgc

clean-lf0:
	rm -rf lf0

clean-cmp:
	rm -rf cmp

clean-gv:
	rm -rf gv

clean-label:
	rm -rf labels/mono
	rm -rf labels/full

clean-mlf:
	rm -f labels/*.mlf

clean-list:
	rm -rf lists

clean-scp:
	rm -rf scp

distclean: clean
	rm -f Makefile

.PHONY: all analysis labels mgc lf0 bap f0 sp ap cmp gv label mlf list scp clean 
