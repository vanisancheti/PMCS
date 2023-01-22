#!/bin/perl


my %vertex_hash = ();
my %edge_hash = ();
$curvertex_lab = 0;
$curedge_lab = 0;
while($line=<>)
{
    chomp($line);
    if($line =~ /^#/)
    {
        #print $line,"\n";
    }
    else
    {
        @array = split(/ /, $line);
        if($array[0] eq "t")
        {
            print $line,"\n";
        }
        else
        {
            if($array[0] eq "v")
            {
                if(exists ($vertex_hash{$array[2]}))
                {
                    print "$array[0] $array[1] $vertex_hash{$array[2]}\n";
                }
                else
                {
                    $vertex_hash{$array[2]} = $curvertex_lab;
                    print "$array[0] $array[1] $vertex_hash{$array[2]}\n";
                    $curvertex_lab++;
                }
            }
            else 
            {
                if($array[0] eq "u")
                {
                    if(exists ($edge_hash{$array[3]}))
                    {
                        print "$array[0] $array[1] $array[2] $edge_hash{$array[3]}\n";
                    }
                    else
                    {
                        $edge_hash{$array[3]} = $curedge_lab;
                        print "$array[0] $array[1] $array[2] $edge_hash{$array[3]}\n";
                        $curedge_lab++;
                    }
                }
            }
        }
    }
}
