#
eval 'exec perl $0 $*'
    if 0;

require Molecule;

##########################################################################

package QCParse;
$debug = 0;

sub testparse {
    my $parse = new QCParse;

    my $string = "x:
 xval
test_basis: STO-3G 6-311G**
charge: 1
method: scf
basis: sto-3g
state: 3b1
molecule:
  H 0 0.0000001 1.00000001
  H 0 0 -1
gradient: yes
optimize: no
frequencies: yes
properties: NPA
y:
yval
z: zval1 zval2
zval3
h:
0 a
1
 2  c";

    print "string:\n--------------\n$string\n--------------\n";

    $parse->parse_string($string);
    $parse->doprint();

    my @t = $parse->value_as_array('h');
    print "-----------------\n";
    for ($i = 0; $i <= $#t; $i++) {
        print "$i: $t[$i]\n";
    }
    print "-----------------\n";

    @t = $parse->value_as_lines('h');
    print "-----------------\n";
    for ($i = 0; $i <= $#t; $i++) {
        print "$i: $t[$i]\n";
    }
    print "-----------------\n";

    my $qcinp = new QCInput($parse);
    my $test_basis = $parse->value("test_basis");
    my @test_basis_a = $parse->value_as_array("test_basis");
    my $state = $qcinp->state();
    my $mult = $qcinp->mult();
    my $method = $qcinp->method();
    my $charge = $qcinp->charge();
    my $basis = $qcinp->basis();
    my $gradient = $qcinp->gradient();
    my $frequencies = $qcinp->frequencies();
    my $optimize = $qcinp->optimize();
    my $natom = $qcinp->n_atom();
    foreach $i (@test_basis_a) {
        print "test_basis_a: $i\n";
    }
    print "test_basis = $test_basis\n";
    print "state = $state\n";
    print "mult = $mult\n";
    print "method = $method\n";
    print "basis = $basis\n";
    print "optimize = $optimize\n";
    print "gradient = $gradient\n";
    print "frequencies = $frequencies\n";
    print "natom = $natom\n";
    for ($i = 0; $i < $natom; $i++) {
        printf "%s %14.8f %14.8f %14.8f\n", $qcinp->element($i),
                                $qcinp->position($i,0),
                                $qcinp->position($i,1),
                                $qcinp->position($i,2);
    }
    printf "qcinp errors: %s\n", $qcinp->error();

    my $inpwr = new MPQCInputWriter($qcinp);
    printf "MPQC input:\n%s", $inpwr->input_string();
}

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize();
    return $self;
}

sub initialize {
    my $self = shift;
    $self->{'keyval'} = {};
}

sub parse_file {
    my $self = shift;
    my $file = shift;
    if (! -f "$file") {
        $self->{"ok"} = 0;
        return;
    }
    open(INPUT, "<$file");
    my $string = "";
    while (<INPUT>) {
        $string = "$string$_";
    }
    close(INPUT);
    #print "Got file:\n$string\n";
    $self->parse_string($string);
    $self->{"ok"} = 1;
}

sub write_file {
    my $self = shift;
    my $file = shift;
    my $keyval = $self->{'keyval'};
    my @keys = keys(%$keyval);
    open(OUTPUT, ">$file");
    foreach $key (@keys) {
        my $value = $keyval->{$key};
        print OUTPUT "${key}:\n";
        print OUTPUT "$value\n";
    }
    close(OUTPUT);
}

sub parse_string {
    my $self = shift;
    my $string = shift;
    my $value = "";
    my $keyword = "";
    $string = "$string\n";
    while ($string) {
        $string =~ s/^[^\n]*\n//;
        $_ = $&;
        s/#.*//;
        if (/^\s*(\w+)\s*:\s*(.*)\s*$/) {
            $self->add($keyword, $value);
            $keyword = $1;
            $value = $2;
        }
        elsif (/^\s*$/) {
            $self->add($keyword, $value);
            $keyword = "";
            $value = "";
        }
        else {
            $value = "$value$_";
        }
    }
    $self->add($keyword, $value);
}

sub add {
    my $self = shift;
    my $keyword = shift;
    my $value = shift;
    if ($keyword ne "") {
        $self->{'keyval'}{$keyword} = $value;
        printf("%s = %s\n", $keyword, $value) if ($debug);
    }
}

# returns the value of the keyword
sub value {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    return $value;
}

# sets the value of the keyword
sub set_value {
    my $self = shift;
    my $keyword = shift;
    my $value = shift;
    my $keyval = $self->{'keyval'};
    $keyval->{$keyword} = $value;
    return $value;
}

# returns the value of the keyword
sub boolean_value {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    $_ = $keyval->{$keyword};
    return "1" if (/^\s*(y|yes|1|true|t)\s*$/i);
    return "0" if (/^\s*(n|no|0|false|f|)\s*$/i);
    "";
}

# returns an array of whitespace delimited tokens
sub value_as_array {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    my @array = ();
    $i = 0;
    $value =~ s/^\s+$//;
    while ($value ne '') {
        $value =~ s/^\s*(\S+)\s*//s;
        $array[$i] = $1;
        $i++;
    }
    return @array;
}

# returns an array reference of whitespace delimited tokens
sub value_as_arrayref {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    my $array = [];
    $i = 0;
    $value =~ s/^\s+$//;
    while ($value ne '') {
        $value =~ s/^\s*(\S+)\s*//s;
        $array->[$i] = $1;
        $i++;
    }
    return $array;
}

# returns an array of lines
sub value_as_lines {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    my @array = ();
    $i = 0;
    while ($value) {
        $value =~ s/^\s*(.*)\s*\n//;
        $array[$i] = $1;
        $i++;
    }
    return @array;
}

# returns 1 if the input file existed
sub ok {
    my $self = shift;
    $self->{"ok"};
}

sub display {
    my $self = shift;
    my @keys = @_ ? @_ : sort keys %$self;
    foreach $key (@keys) {
        print "\t$key => $self->{$key}\n";
    }
}

sub doprint {
    my $self = shift;
    print "QCParse:\n";
    my $keyval = $self->{'keyval'};
    foreach $i (keys %$keyval) {
        my $val = $keyval->{$i};
        $val =~ s/\n/\\n/g;
        print "keyword = $i, value = $val\n";
    }
}

##########################################################################

package QCInput;
$debug = 0;

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize {
    my $self = shift;
    my $parser = shift;
    if ($parser eq "") {
        $parser = new QCParse;
    }
    $self->{"parser"} = $parser;
    $self->{"error"} = "";

    $self->{"molecule"} = new Molecule($parser->value("molecule"));
}

sub error {
    my $self = shift;
    my $msg = shift;
    $self->{"error"} = "$self->{'error'}$msg";
}

sub checkpoint {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("checkpoint");
    my $val = $self->{"parser"}->value("checkpoint");
    if ($val ne "" && $bval eq "") {
        $self->error("Bad value for checkpoint: $val");
        $bval = "0";
    }
    elsif ($val eq "") {
        $bval = "1";
    }
    $bval;
}

sub label {
    my $self = shift;
    $self->{"parser"}->value("label");
}

sub charge {
    my $self = shift;
    $_ = $self->{"parser"}->value("charge");
    s/^\s+//;
    s/\s+$//;
    if (! /\d+/) {
        $self->error("Bad charge: $_ (using 0)\n");
        $_ = "0";
    }
    $_;
}

sub method {
    my $self = shift;
    $_ = $self->{"parser"}->value("method");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $self->error("No method given (using default).\n");
        $_ = "SCF";
    }
    tr/a-z/A-Z/;
    $_;
}

sub symmetry {
    my $self = shift;
    $_ = $self->{"parser"}->value("symmetry");
    s/^\s*//;
    s/\s*$//;
    uc $_;
}

sub memory {
    my $self = shift;
    $_ = $self->{"parser"}->value("memory");
    s/^\s*//;
    s/\s*$//;
    if ($_ eq "") {
        $_ = 16000000;
    }
    $_;
}

sub state {
    my $self = shift;
    $_ = $self->{"parser"}->value("state");
    s/^\s*//;
    s/\s*$//;
    uc $_;
}

sub mult {
    my $self = shift;
    $_ = $self->state();
    s/(\d)\D.*$/\1/;
    if (/^\s*$/) {
        $_ = 1;
    }
    $_;
}

sub basis {
    my $self = shift;
    $_ = $self->{"parser"}->value("basis");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $self->error("No basis given (using default).\n");
        $_ = "STO-3G";
    }
    tr/a-z/A-Z/;
    $_;
}

sub gradient {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("gradient");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("gradient");
        $self->error("Bad value for gradient: $val");
    }
    $bval;
}

sub fzc {
    my $self = shift;
    $_ = $self->{"parser"}->value("fzc");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $_ = 0;
    }
    $_;
}

sub fzv {
    my $self = shift;
    $_ = $self->{"parser"}->value("fzv");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $_ = 0;
    }
    $_;
}

sub optimize {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("optimize");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("optimize");
        $self->error("Bad value for optimize: $val");
    }
    $bval;
}

sub transition_state {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("transition_state");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("transition_state");
        $self->error("Bad value for transtion_state: $val");
    }
    $bval;
}

sub frequencies {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("frequencies");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("frequencies");
        $self->error("Bad value for frequencies: $val");
    }
    $bval;
}

sub axyz_lines {
    my $self = shift;
    $self->molecule()->string();
}

sub molecule() {
    my $self = shift;
    return $self->{"molecule"};
}

sub n_atom {
    my $self = shift;
    printf "QCInput: returning natom = %d\n", $self->{"natom"} if ($debug);
    $self->molecule()->n_atom();
}

sub element {
    my $self = shift;
    $self->molecule()->element(@_);
}

sub position {
    my $self = shift;
    $self->molecule()->position(@_);
}

sub write_file {
    my $self = shift;
    my $file = shift;
    my $parser = $self->{'parser'};
    $parser->write_file($file);
}

sub mode_following() {
    my $self = shift;
    return scalar($self->{"parser"}->value_as_array("followed")) != 0;
}

# returns 1 if the input file existed
sub ok {
    my $self = shift;
    $self->{"parser"}->{"ok"};
}

##########################################################################

package InputWriter;

# Input Writer is abstract
sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize() {
    my $self = shift;
    my $qcinput = shift;
    $self->{"qcinput"} = $qcinput;
}

# this should be overridden
sub input_string() {
    "";
}

sub write_input() {
    my $self = shift;
    my $file = shift;
    my $input = $self->input_string();
    open(OUTPUT,">$file");
    printf OUTPUT "%s", $input;
    close(OUTPUT);
}

sub write_qcinput {
    my $self = shift;
    my $file = shift;
    my $qcinput = $self->{'qcinput'};
    $qcinput->write_file($file);
}

##########################################################################

package MPQCInputWriter;
@ISA = qw( InputWriter );
%methodmap = ("MP2" => "MBPT2",
              "OPT1[2]" => "MBPT2",
              "OPT2[2]" => "MBPT2",
              "ZAPT2" => "MBPT2",
              "ROSCF" => "SCF",
              "SCF" => "SCF" );
%mbpt2map = ("MP2" => "mp",
             "OPT1[2]" => "opt1",
             "OPT2[2]" => "opt2",
             "ZAPT2" => "zapt");
$debug = 0;

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;

    $self->initialize(@_);
    return $self;
}

sub initialize() {
    my $self = shift;
    my $qcinput = shift;
    $self->{"qcinput"} = $qcinput;
}

sub input_string() {
    my $self = shift;
    my $qcinput = $self->{"qcinput"};
    my $qcparse = $qcinput->{"parser"};

    printf "molecule = %s\n", $qcparse->value("molecule") if ($debug);

    my $symmetry = $qcinput->symmetry();
    my $mol = "% molecule specification";
    $mol = "$mol\nmolecule<Molecule>: (";
    $mol = "$mol\n  symmetry = $symmetry";
    $mol = "$mol\n  angstroms = yes";
    $mol = "$mol\n  { atoms geometry } = {";
    printf "MPQCInputWriter: natom = %d\n", $qcinput->n_atom() if ($debug);
    my $i;
    for ($i = 0; $i < $qcinput->n_atom(); $i++) {
        $mol = sprintf "%s\n    %s     [ %14.8f %14.8f %14.8f ]",
                       $mol, $qcinput->element($i),
                       $qcinput->position($i,0),
                       $qcinput->position($i,1),
                       $qcinput->position($i,2);
    }
    $mol = "$mol\n  }";
    $mol = "$mol\n)\n";

    my $basis = "% basis set specification";
    $basis = "$basis\nbasis<GaussianBasisSet>: (";
    $basis = sprintf "%s\n  name = \"%s\"", $basis, $qcinput->basis();
    $basis = "$basis\n  molecule = \$:molecule";
    $basis = "$basis\n)\n";

    my $fixed = $qcparse->value_as_arrayref("fixed");
    my $followed = $qcparse->value_as_arrayref("followed");
    if (scalar(@{$fixed}) != 0) {
        $fixed = $self->mpqc_fixed_coor($fixed);
    }
    else {
        $fixed = "";
    }
    if (scalar(@{$followed}) != 0) {
        $followed = $self->mpqc_followed_coor($followed);
    }
    else {
        $followed = "";
    }

    my $coor = "  % molecular coordinates for optimization";
    $coor = "$coor\n  coor<SymmMolecularCoor>: (";
    $coor = "$coor\n    molecule = \$:molecule";
    $coor = "$coor\n    generator<IntCoorGen>: (";
    $coor = "$coor\n      molecule = \$:molecule";
    $coor = "$coor\n    )";
    $coor = "$coor$followed";
    $coor = "$coor$fixed";
    $coor = "$coor\n  )\n";

    my $memory = $qcinput->memory();
    my $method = $methodmap{uc($qcinput->method())};
    $method = "SCF" if ($method eq "");
    if ($method eq "SCF") {
        if ($qcinput->mult() == 1) {
            $method = "CLSCF";
        }
        else {
            $method = "HSOSSCF";
        }
    }
    my $mole = "  do_energy = yes";
    if ($qcinput->gradient()) {
        $mole = "$mole\n  do_gradient = yes";
    }
    else {
        $mole = "$mole\n  do_gradient = no";
    }
    $mole = "$mole\n  % method for computing the molecule's energy";
    $mole = "$mole\n  mole<$method>: (";
    $mole = "$mole\n    molecule = \$:molecule";
    $mole = "$mole\n    basis = \$:basis";
    $mole = "$mole\n    coor = \$..:coor";
    $mole = "$mole\n    memory = $memory";
    if ($method eq "MBPT2") {
        my $fzc = $qcinput->fzc();
        my $fzv = $qcinput->fzv();
        my $mbpt2method = $mbpt2map{uc($qcinput->method())};
        $mole = "$mole\n    method = $mbpt2method";
        $mole = "$mole\n    nfzc = $fzc";
        $mole = "$mole\n    nfzv = $fzv";
        my $refmethod = "";
        if ($qcinput->mult() == 1) {
            $refmethod = "CLSCF";
        }
        else {
            $refmethod = "HSOSSCF";
        }
        $mole = "$mole\n    reference<$refmethod>: (";
        $mole = "$mole\n      molecule = \$:molecule";
        $mole = "$mole\n      basis = \$:basis";
        $mole = "$mole\n      memory = $memory";
        if (! ($basis =~ /^STO/
               || $basis =~ /^MI/
               || $basis =~ /^\d-\d1G$/)) {
            $mole = "$mole\n      guess_wavefunction<$refmethod>: (";
            $mole = "$mole\n        molecule = \$:molecule";
            $mole = "$mole\n        basis<GaussianBasisSet>: (";
            $mole = "$mole\n          molecule = \$:molecule";
            $mole = "$mole\n          name = \"STO-3G\"";
            $mole = "$mole\n        )";
            $mole = "$mole\n        memory = $memory";
            $mole = "$mole\n      )";
        }
        $mole = "$mole\n    )";
    }
    elsif (! ($basis =~ /^STO/
              || $basis =~ /^MI/
              || $basis =~ /^\d-\d1G$/)) {
        $mole = "$mole\n    guess_wavefunction<$method>: (";
        $mole = "$mole\n      molecule = \$:molecule";
        $mole = "$mole\n      basis<GaussianBasisSet>: (";
        $mole = "$mole\n        molecule = \$:molecule";
        $mole = "$mole\n        name = \"STO-3G\"";
        $mole = "$mole\n      )";
        $mole = "$mole\n      memory = $memory";
        $mole = "$mole\n    )";
    }
    $mole = "$mole\n  )\n";

    my $opt;
    if ($qcinput->optimize()) {
        $opt = "  optimize = yes";
    }
    else {
        $opt = "  optimize = no";
    }
    my $optclass, $updateclass;
    if ($qcinput->transition_state()) {
        $optclass = "EFCOpt";
        $updateclass = "PowellUpdate";
    }
    else {
        $optclass = "QNewtonOpt";
        $updateclass = "BFGSUpdate";
    }
    $opt = "$opt\n  % optimizer object for the molecular geometry";
    $opt = "$opt\n  opt<$optclass>: (";
    $opt = "$opt\n    max_iterations = 20";
    $opt = "$opt\n    function = \$..:mole";
    if ($qcinput->transition_state()) {
        $opt = "$opt\n    transition_state = yes";
        if ($qcinput->mode_following()) {
            $opt = "$opt\n    hessian = [ [ -0.1 ] ]";
            $opt = "$opt\n    mode_following = yes";
        }
    }
    $opt = "$opt\n    update<$updateclass>: ()";
    $opt = "$opt\n    convergence<MolEnergyConvergence>: (";
    $opt = "$opt\n      cartesian = yes";
    $opt = "$opt\n    )";
    $opt = "$opt\n  )\n";

    my $freq = "";
    if ($qcinput->frequencies()) {
        $freq = "% vibrational frequency input";
        $freq = "$freq\n  freq<MolecularFrequencies>: (";
        $freq = "$freq\n    molecule = \$:molecule";
        $freq = "$freq\n  )\n";
    }

    my $mpqcstart = sprintf ("mpqc: (\n  checkpoint = %s\n",
                             bool_to_yesno($qcinput->checkpoint()));
    $mpqcstart = sprintf ("%s  savestate = %s\n",
                          $mpqcstart,bool_to_yesno($qcinput->checkpoint()));
    my $mpqcstop = ")\n";
    "$mol$basis$mpqcstart$coor$mole$opt$freq$mpqcstop";
}

sub mpqc_fixed_coor {
    my $self = shift;
    my $coorref = shift;
    my $result = "";
    $result = "\n    fixed<SetIntCoor>: [";
    while (scalar(@{$coorref}) != 0) {
        my $nextcoor = $self->mpqc_sum_coor("      ","",$coorref);
        $result = "$result\n$nextcoor";
    }
    $result = "$result\n    ]";
}

sub mpqc_followed_coor {
    my $self = shift;
    my $coorref = shift;
    sprintf "\n%s", $self->mpqc_sum_coor("    ","followed",$coorref);
}

sub mpqc_sum_coor {
    my $self = shift;
    my $space = shift;
    my $name = shift;
    my $coor = shift;
    my $result = "$space$name<SumIntCoor>:(";
    $result = "$result\n$space  coor: [";
    my @coef = ();
    do {
        $coef[$ncoor] = shift @{$coor};
        my $simple = $self->mpqc_coor($coor);
        $result = "$result\n$space    $simple";
        $ncoor = $ncoor + 1;
    } while($coor->[0] eq "+" && shift @{$coor} eq "+");
    $result = "$result\n$space  ]";
    $result = "$result\n$space  coef = [";
    my $i;
    foreach $i (0..$#coef) {
        $result = "$result $coef[$i]";
    }
    $result = "$result]";
    $result = "$result\n$space)";
    $result;
}

sub mpqc_coor {
    my $self = shift;
    my $coor = shift;
    my $type = shift @{$coor};
    if ($type eq "TORS") {
        return sprintf "<TorsSimpleCo>:(atoms = [%d %d %d %d])",
                       shift @{$coor},shift @{$coor},
                       shift @{$coor},shift @{$coor};
    }
    if ($type eq "BEND") {
        return sprintf "<BendSimpleCo>:(atoms = [%d %d %d])",
                       shift @{$coor},shift @{$coor},
                       shift @{$coor};
    }
    if ($type eq "STRE") {
        return sprintf "<StreSimpleCo>:(atoms = [%d %d])",
                        shift @{$coor},shift @{$coor};
    }
}

sub bool_to_yesno {
    if (shift) { return "yes"; }
    else { return "no"; }
}

##########################################################################

package G94InputWriter;
@ISA = qw( InputWriter );
%methodmap = ("MP2" => "MP2",
              "OPT2[2]" => "MP2",
              "ZAPT2" => "MB2",
              "SCF" => "SCF",
              "ROSCF" => "ROSCF"
              );

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize() {
    my $self = shift;
    my $qcinput = shift;
    $self->{"qcinput"} = $qcinput;
}

sub input_string() {
    my $self = shift;
    my $qcinput = $self->{"qcinput"};
    my $qcparse = $qcinput->{"parser"};
    my $extra = "";

    my $method = $methodmap{$qcinput->method()};
    my $basis = $qcinput->basis();
    if ($method eq "MP2") {
        my $window = "full";
        if ($qcinput->fzc() != 0 || $qcinput->fzv() != 0) {
            $window = "rw";
            $extra = sprintf("\n%s %d %d", $extra,
                             $qcinput->fzc()+1, - $qcinput->fzv());
        }
        $method = "$method($window)";
    }
    my $route = sprintf("# %s/%s", $method, $basis);
    if ($qcinput->optimize()) { $route = "$route opt" }
    if ($qcinput->frequencies()) { $route = "$route freq" }

    my $label = $qcinput->label();
    $label = "Input generated by G94InputWriter" if ($label eq "");

    my $chargemult = sprintf("%s %s", $qcinput->charge(), $qcinput->mult());

    my $mol = $qcinput->axyz_lines();

    my $com = "$route\n\n$label\n\n$chargemult\n$mol$extra\n\n";

    $com;
}
