#!/bin/sh
[ "$(git ls-remote --heads origin coverity_scan | wc -l)" -gt 0 ] &&  git push github :coverity_scan
git show-ref --heads --quiet coverity_scan && git branch -D coverity_scan
git checkout --orphan coverity_scan

cat << EOF > .travis.yml
language: c

env:
  global:
    - secure: "DnhraXW8wdrP5UzvQbAtarIUFk0i78py+in2VHphS7PG4Tp2qmAyAcf3NNZTm75E/vDQbeGQpEPHORxYxHr3KLfhpqeumQ+o/TzGD42zNHOedSKmr4khVkYohAaI7yC/+j/W5ndvnohMdrNAkdxVMg6g1HmY8nqnn41qHzebqzpFytxbBQpNe4pCxHZb4ujArxFIZcaYSudQS5CpdEzVafZvtcqLyZzrgpH+ys8AYxbspdHT5VvY/cG4fZZyfz9v3I/YadP+/Yes+CbAeZHdv3+2vBiXllHspcfkSA5yS93Mmzm0zT1obl3F1TCIQG7BDML96haZfW7sqcYXMnJfK5TamUGB1pVDf8kV10rm+Fc8jlnmmdQENktaPZs+5Y4SEcFdc9mgjDPjvxQ1vXr5LkJs/daNzhhK8psSy/PcVKKrpuHEc9I3VVE8MN81f+HYIdkM428H7H8nUqAxutvvrIES3ZA2U/+Yyzb4i+G1FQGzynM3YJkjoSWoXMjoMY9Mu1Posvbs6bAgS7nHOX2IHS+WUezjEb4A9eL7nfIn1WMMFKTv73yIVsykjPCNnQBasZakleRgsUrcqvRDR3ak42xjOlTw+PN07F/3szuA8Uk06us/6F2kABu+FZmX+hkHmONmFRF5biB2T4nyWZqoQgB/DB+G/kQ4On+WmvMkSkk="
    - secure: "wfw4th54cfcaGP8AIuAR65KJvcTNYbtl1Hh09biCC1q7uZH7dztvvPi+FvDnw3/W2ZDEOPdz7BcGd/Byap/8H9iFH0WIx95PyiByD6QXLgkyRgQ+fEYJ3DvwCcT0iq94/trptz6M0kMj5hL49nfpW4h3GJisDJanTgFILnogred1+1vj0QR9bsVcBEW1um/TiMppFedPnElz0CqVSrsFWyFLOpVvnB8lq1WWEz0anAIwYDblTxOfLp9rNjRo97+GbwuiuUQQs0LSMIkP/mUW5YAttkxnF8w+1zzq+UBRSfve/8nG0Lz99hXD0IHHUa2eUxlU/EkAIISt6uHFV9RILKARsqFHEeL8sxgpi6fOnKS4uY53T69rg8UD4Y4uGmhKeyzEJ8aeIOXKvznvO8KwnMNQePnDKg6LxGWlcQTS8GZyJWuL7jTzPhxqwb9Nb1p9JmaYFCoAdNvS39TqWPApzJ31e3SsN/LAXu3veDZDJaeUM4xX5sGYRCoEpK3mD978jTljap6jIwOq2QOKCG4qDi0twlA8E6LmQxnTrNKddW29/koR50d6UZ7JBOqtj4WPdelhjRRIpOkbU19xPOjp+mTtx+HKPjpIHGvq0N1V1AH8D/DWsoqPk1Upv4CvC6vAmIh2IsdL30dBdf0v7KXuv4UIl3dSHEZf/RLc0AAWQ8M="
    - ALSA_D=0
    - SOUNDPIPE_D=0
    - SPA_D=0
    - SNDFILE_D=0
    - SOUNDPIPE_DATA_DIR=Soundpipe/modules/data
    - SOUNDPIPE_LIB=Soundpipe/libsoundpipe.a
    - SOUNDPIPE_INC=-ISoundpipe/h
    - DATA=Soundpipe/modules/data
    - BISON_VERSION=bison-3.0.4
    - GWION_DOC_DIR=doc
    - GWION_API_DIR=api
    - GWION_TOK_DIR=tok
    - GWION_TAG_DIR=tag
    - GWION_ADD_DIR=add
    - USE_DOUBLE=0
    - SP_BRANCH=dev

before_install:
      - echo -n | openssl s_client -connect scan.coverity.com:443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' | sudo tee -a /etc/ssl/certs/ca-

before_install:
  - sudo apt-get install libsndfile1 libsndfile1-dev lua5.2

before_script:
  - export CFLAGS+=-I$TRAVIS_BUILD_DIR/Soundpipe/h
  - bash util/travis_prepare.sh

script:
  - ./configure; make

addons:
  coverity_scan:
    project:
      name: "fennecdjay/Gwion"
      description: "Build submitted via Travis CI"
    notification_email: astor.jeremie@wanadoo.fr
    build_command_prepend: "echo prepare; ./configure"
    build_command:   "make"
    branch_pattern: coverity_scan
EOF

git add .travis.yml
git commit -m"push to Travis to coverity"
git push --set-upstream github coverity_scan
git checkout dev
