#!/usr/bin/perl -w
open F, "<data" or die;
foreach $line (<F>) {
	chomp $line;
	($name,$full) = split /,/,$line;
	$cities{$name} = $full;
}
foreach $line (<>) {
	chomp $line;
	@words = split / /,$line;
	$line_to_print = '';
	foreach $word (@words) {
		if (defined $cities{$word}) {
			$line_to_print .= "$cities{$word} ($word) ";
		} else {
			$line_to_print .= "$word ";
		}
	}
	$line_to_print .= "\n";
	print "$line_to_print";
	
}
