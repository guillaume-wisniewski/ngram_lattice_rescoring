pSimple program to find the path in a lattice with the highest LM score and extract the corresponding string.

============
Dependencies
============

This program requires the following libraries:

* `openFST <http://www.openfst.org/twiki/bin/view/FST/WebHome>`_ a library for FST (tested with version 1.5.3);
* `openGRM <http://www.opengrm.org/>`_ an extension of :code:`openFST` to estimate and manipulate language models represented by FST (tested with version 1.3.0) ;
* `BOOST <https://www.boost.org/users/download/>`_ a library to make C++ usable (tested with version 1_69_0). 

=====
Usage
=====

where:

* :code:`lm` is a the name of the language model stored in a
  FST. Converting an ARPA file to a FST or estimating directly a LM in a
  FST can be done thanks to t he :code:`openGRM` toolkit.
* :code:`far` is the lattices to score stored in a FST archive
* :code:`output` is the name of the file in which the strings will be
  dumped.
  
LM estimation with openGRM
==========================

There are two ways to create a LM using the :code:`openGRM`. The first
one consists in using any standard toolkit to generate a language
model (in ARPA) and than convert it to a FST using the
:code:`ngramread` command:

.. code-block:: bash

   ngramread --ARPA earnest.ARPA >earnest.mod

In all my experiments this command resulted in an error.

The :code:`openGRM` toolkit also provides a pipeline to directly
estimate a language model:

.. code-block:: bash

   # Example corpus
   wget http://www.openfst.org/twiki/pub/FST/FstExamples/wotw.txt

   # Create vocabulary file
   ngramsymbols < wotw.txt > voc.syms

   # Estimate LM
   farcompilestrings -symbols=voc.syms -keep_symbols=1 wotw.txt > wotw.far
   ngramcount -order=3 wotw.far > wotw.cnt
   ngrammake wotw.cnt > en3gram.mod


Lattices preparations
---------------------

:code:`XXX` takes as input a :code:`far` containing all the lattices
to be scores. This lattices **must** use the same vocabulary as the
language model and oov **must** manually be mapped to **<unk>**.

This far can be created as follows:

.. code-block:: bash

   # `data' is a directory containing all lattices; each lattice is
   # stored in a file following the pattern `lattice_[0-9]+.txt`
   # All oov must be replaced by the <unk> symbol

   # compile all file
   # the vocabulary file **must** be the same as the one used to
   # generate the LM
   for fn in `ls data/*txt`; do echo $fn && ../bin/fstcompile --keep_isymbols --keep_osymbols --isymbols=voc.syms --osymbols=voc.syms $fn ${fn/txt/fst}; done

   # create the FAR
   # Be careful: the files must be ordered in lexicographic order
   farcreate *.fst mj2_test35.far   

=======
Install
=======

Installing the dependencies
===========================

The following instructions describe the dependencies can be installed:

.. code-block:: bash

  # all dependencies will be installed in this directory (with the usual layout: bin, include, ...)
  PREFIX=/people/wisniews/workspace/lattice_rescoring

  # The last version of the two libraries
  wget http://www.openfst.org/twiki/pub/GRM/NGramDownload/opengrm-ngram-1.3.0.tar.gz
  wget http://www.openfst.org/twiki/pub/FST/FstDownload/openfst-1.5.3.tar.gz

  # Install openFST with FAR support (and binaries)
  tar xvfz openfst-1.5.3.tar.gz
  cd openfst-1.5.3
  ./configure --enable-bin --enable-far --prefix=$PREFIX
  make install

  # The only trick is to indicate to openGRM where openFST has been installed
  tar xvfz opengrm-ngram-1.3.0.tar.gz
  CXXFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib/ ./configure --prefix=$PREFIX
  make install

  # Install boost without python as it makes compilation fail on my mac
  wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
  tar xvfz boost_1_69_0.tar.bz2
  cd boost_1_69_0
  ./bootstrap.sh --without-libraries=python --prefix=~/workspace/ngram_lattice_rescoring/
  ./b2 install


Installing :code:`ngram_lattice`
================================

1. Edit :filename:`Makefile` to specify where :code:`BOOST`,
   :code:`openFST` and :code:`openGRM` are installed. Supposidely you
   just have to set the :code:`PREFIX` variable;
1. Simply :code:`make`.

====
TODO
====

[ ] check if openFST correctly deals with UTF-8
[X] check if <unk> and <eps> are used correctly
[X] correctly deal with backoff arcs
