#!/bin/bash -xe

#cvs -d :pserver:tgdchmi2@nero.trillian.ch:/data/projects/kphotobook/cvs export -r HEAD kphotobookdoc/www
cvs -d :ext:starcube@cvs1:/cvsroot/kphotobook export -r HEAD kphotobookdoc/www
rm -rf htdocs/*
mv kphotobookdoc/www/* htdocs
chmod -R g+w htdocs/*
rmdir kphotobookdoc/www/
rmdir kphotobookdoc
