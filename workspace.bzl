# default paths
DEFAULT_BOOST_URL="http://downloads.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.tar.gz"

# set up spotify-json workspace
def spotify_json_library(prefix=None, has_boost=False, boost_url=None):
    '''
    Macro for setup of the spotify-json library, independently or as part of
    a larger workspace.

    Usage: spotify_json_library()
    Options:
    - prefix (String): if this library is nested within a larger workspace,
            the path to this library should be provided here
    - has_boost (Boolean): if Boost exists in the workspace as "@boost//:boost",
    	   it can be used by passing in "True"
    - boost_url (String): if the user would like to obtain Boost from a URL other
            than the default, this alternate URL can be passed in
    '''
    add_external_repos(prefix=prefix, has_boost=has_boost, boost_url=boost_url)

# add a prefix to a path
def _add_prefix(path, prefix=None):
    if not prefix:
        return path
    if not prefix.endswith('/'):
        prefix = prefix + '/'
    return prefix + path

# set up external repos
def add_external_repos(prefix=None, has_boost=False, boost_url=None):
    native.bind(
        name = "double-conversion",
        actual = "//" + _add_prefix("vendor/double-conversion:double-conversion", prefix)
    )
    
    if has_boost:
    	return

    if not boost_url:
    	boost_url = DEFAULT_BOOST_URL

    native.new_http_archive(
        name = "boost",
        url = DEFAULT_BOOST_URL,
        build_file = _add_prefix("vendor/boost/boost.BUILD", prefix),
        strip_prefix = "boost_1_61_0",
    )
