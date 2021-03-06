package LWP::Protocol::GHTTP;

# $Id: GHTTP.pm,v 1.1 2000/11/30 05:59:15 gisle Exp $

#
# You can tell LWP to use this module for 'http' requests by running
# code like this before you make requests:
#
#    require LWP::Protocol::GHTTP;
#    LWP::Protocol::implementor('http', 'LWP::Protocol::GHTTP');
#

use strict;
use vars qw(@ISA);

require LWP::Protocol;
@ISA=qw(LWP::Protocol);

require HTTP::Response;
require HTTP::Status;

use HTTP::GHTTP qw(METHOD_GET METHOD_HEAD METHOD_POST);

my %METHOD =
(
 GET  => METHOD_GET,
 HEAD => METHOD_HEAD,
 POST => METHOD_POST,
);

sub request
{
    my($self, $request, $proxy, $arg, $size, $timeout) = @_;

    my $method = $request->method;
    unless (exists $METHOD{$method}) {
	return HTTP::Response->new(&HTTP::Status::RC_BAD_REQUEST,
				   "Bad method '$method'");
    }

    my $r = HTTP::GHTTP->new($request->uri);

    # XXX what headers for repeated headers here?
    $request->headers->scan(sub { $r->set_header(@_)});

    $r->set_type($METHOD{$method});

    # XXX should also deal with subroutine content.
    my $cref = $request->content_ref;
    $r->set_body($$cref) if length($$cref);

    # XXX is this right
    $r->set_proxy($proxy->as_string) if $proxy;

    $r->process_request;

    my $response = HTTP::Response->new($r->get_status);

    # XXX How can get the headers out of $r??  This way is too stupid.
    for (qw(Date Server Content-type Last-Modified ETag)) {
	my $v = $r->get_header($_);
	$response->header($_ => $v) if $v;
    }

    return $self->collect_once($arg, $response, $r->get_body);
}

1;
