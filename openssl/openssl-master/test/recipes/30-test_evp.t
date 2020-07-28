#! /usr/bin/env perl
# Copyright 2015-2020 The OpenSSL Project Authors. All Rights Reserved.
#
# Licensed under the Apache License 2.0 (the "License").  You may not use
# this file except in compliance with the License.  You can obtain a copy
# in the file LICENSE in the source distribution or at
# https://www.openssl.org/source/license.html


use strict;
use warnings;

use OpenSSL::Test qw(:DEFAULT data_file bldtop_dir srctop_file srctop_dir bldtop_file);
use OpenSSL::Test::Utils;

BEGIN {
setup("test_evp");
}

use lib srctop_dir('Configurations');
use lib bldtop_dir('.');
use platform;

my $no_fips = disabled('fips') || ($ENV{NO_FIPS} // 0);
my $no_legacy = disabled('legacy') || ($ENV{NO_LEGACY} // 0);

# Default config depends on if the legacy module is built or not
my $defaultcnf = $no_legacy ? 'default.cnf' : 'default-and-legacy.cnf';

my @configs = ( $defaultcnf );
# Only add the FIPS config if the FIPS module has been built
push @configs, 'fips.cnf' unless $no_fips;

my @files = qw( evprand.txt evpciph.txt evpdigest.txt evppkey.txt
    evppkey_ecc.txt evpciph_aes_cts.txt);

my @defltfiles = qw( evpencod.txt evpkdf.txt evppkey_kdf.txt evpmac.txt
    evppbe.txt evpcase.txt evpccmcavs.txt );
my @ideafiles = qw( evpciph_idea.txt );
push @defltfiles, @ideafiles unless disabled("idea");

my @sivfiles = qw( evpaessiv.txt );
push @defltfiles, @sivfiles unless disabled("siv");

my @castfiles = qw( evpciph_cast5.txt );
push @defltfiles, @castfiles unless disabled("cast");

my @seedfiles = qw( evpciph_seed.txt );
push @defltfiles, @seedfiles unless disabled("seed");

my @sm4files = qw( evpciph_sm4.txt );
push @defltfiles, @sm4files unless disabled("sm4");

my @desfiles = qw( evpciph_des.txt );
push @defltfiles, @desfiles unless disabled("des");

my @rc4files = qw( evpciph_rc4.txt );
push @defltfiles, @rc4files unless disabled("rc4");

my @rc5files = qw( evpciph_rc5.txt );
push @defltfiles, @rc5files unless disabled("rc5");

my @rc2files = qw( evpciph_rc2.txt );
push @defltfiles, @rc2files unless disabled("rc2");

my @chachafiles = qw( evpciph_chacha.txt );
push @defltfiles, @chachafiles unless disabled("chacha");

my @bffiles = qw( evpciph_bf.txt );
push @defltfiles, @bffiles unless disabled("bf");

my @md2files = qw( evpmd_md2.txt );
push @defltfiles, @md2files unless disabled("md2");

my @mdc2files = qw( evpmd_mdc2.txt );
push @defltfiles, @mdc2files unless disabled("mdc2");

plan tests =>
    ($no_fips ? 0 : 1)          # FIPS install test
    + (scalar(@configs) * scalar(@files))
    + scalar(@defltfiles);

unless ($no_fips) {
    my $infile = bldtop_file('providers', platform->dso('fips'));

    ok(run(app(['openssl', 'fipsinstall',
                '-out', bldtop_file('providers', 'fipsmodule.cnf'),
                '-module', $infile])),
       "fipsinstall");
}

foreach (@configs) {
    $ENV{OPENSSL_CONF} = srctop_file("test", $_);

    foreach my $f ( @files ) {
        ok(run(test(["evp_test", data_file("$f")])),
           "running evp_test $f");
    }
}

#TODO(3.0): As more operations are converted to providers we can move more of
#           these tests to the loop above

$ENV{OPENSSL_CONF} = srctop_file("test", $defaultcnf);

foreach my $f ( @defltfiles ) {
    ok(run(test(["evp_test", data_file("$f")])),
       "running evp_test $f");
}
