<?php
require_once BASEPATH . '/libpp/lib/Request/classes.inc';

class PXRequestUserTest extends UnitTestCase {
    
    private static $original_bucks_server;

    function setUp() {
        self::$original_bucks_server = $_SERVER;
        self::setUp_http_env();
    }
    
    /* restore original environment after each test */
    function tearDown() {
        $_SERVER = self::$original_bucks_server;
    }

    function test_Instance_Should_be_PXRequestUser() {
        $request = new PXRequestUser();
        $this->assertIsA($request, 'PXRequestUser');
    }
    
    function test_default_HTTPMethod_should_be_GET() {
        $this->assertEqual(PXRequestUser::GetHttpMethod(), 'GET');
    }
    
    function test_default_HTTPReferer_should_be_defined() {
        $this->assertEqual(PXRequestUser::getHttpReferer(), 'http://example.com/test/referer');
    }
    
    //TODO: why should we want two referer functions?
    function test_default_Referer_should_be_set() {
        $request = new PXRequestUser();
        $this->assertEqual($request->getReferer(), 'http://example.com/test/referer');
    }

    function test_default_HTTPRequestURI_should_be_defined() {
        $this->assertEqual(PXRequestUser::getRequestUri(), '/path/to/test/index.phtml?a=1&b=2');
    }

    function test_GET_HTTPMethod_should_be_GET() {
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $this->assertEqual(PXRequestUser::getHttpMethod(), 'GET');
    }
    
    function test_POST_HTTPMethod_should_be_POST() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $this->assertEqual(PXRequestUser::getHttpMethod(), 'POST');
    }

    /* 
     *
     * PXRequestUser->getPath()
     *
     */
    function test_GetPath_should_ignore_index_phtml() {
        $request = new PXRequestUser();
        $path_parts = array('path', 'to', 'test');
        $this->assertEqual($request->getPath(), $path_parts);
    }
    
    function test_GetPath_should_ignore_known_indexes() {
        $known_indexes = array(
            'index.html',  'index.shtml', 
            'index.phtml', 'index.php', 
            'index.php3',  'index.php4', 
            'index.jsp', 
            'default.htm', 'default.asp');
            
        foreach($known_indexes as $index) {
            $_SERVER['SCRIPT_NAME'] = '/path/to/test/' . $index;
            
            $request = new PXRequestUser();
            $path_parts = array('path', 'to', 'test');
            $this->assertEqual($request->getPath(), $path_parts);
        }
    }

    function test_GetPath_should_not_ignore_other_extensions() {
        $_SERVER['SCRIPT_NAME'] = '/path/to/test/readme.doc';
        $request = new PXRequestUser();
        $path_parts = array('path', 'to', 'test', 'readme.doc');
        $this->assertEqual($request->getPath(), $path_parts);
    }


    /*
     *
     * PXRequestUser::getHttpHost() 
     *
     */
    function test_default_getHttpHost_sould_be_defined() {
        $this->assertEqual(PXRequestUser::getHttpHost(), 'test.example.com');
    }

    function test_getHttpHost_sould_be_overrided() {
        $_SERVER['HTTP_X_HOST'] = 'overrided.example.com';
        $this->assertEqual(PXRequestUser::getHttpHost(), 'overrided.example.com');
    }

    function test_getHttpHost_sould_handle_multiple_hosts() {
        $_SERVER['HTTP_X_HOST'] = 'maybe.example.com, correct.example.com';
        $this->assertEqual(PXRequestUser::getHttpHost(), 'correct.example.com');
    }
    
    /*
     *
     * PXRequestUser::getRemoteAddr()
     *
     */
    function test_default_getRemoteAddr_sould_be_defined() {
        $this->assertEqual(PXRequestUser::getRemoteAddr(), '10.1.1.100');
    }
    
    function test_getRemoteAddr_sould_be_overrided() {
        $_SERVER['HTTP_X_REAL_IP'] = '192.168.1.1';
        $this->assertEqual(PXRequestUser::getRemoteAddr(), '192.168.1.1');
    }
    
    function test_getRemoteAddr_sould_handle_multiple_addreses() {
        $_SERVER['HTTP_X_REAL_IP'] = '10.10.10.10, 20.20.20.20, 192.168.1.1';
        $this->assertEqual(PXRequestUser::getRemoteAddr(), '192.168.1.1');
    }




    /*
     *
     * Private
     *
     */
    private static function setUp_http_env() {
        $_SERVER['REQUEST_METHOD']  = 'GET';
        $_SERVER['QUERY_STRING']    = 'a=1&b=2';
        $_SERVER['REQUEST_URI']     = '/path/to/test/index.phtml?a=1&b=2';
        $_SERVER['DOCUMENT_ROOT']   = '/home/test/site';
        $_SERVER['SCRIPT_FILENAME'] = '/home/test/site/libpp/htdocs/test/index.phtml';
        $_SERVER['SCRIPT_NAME']     = '/path/to/test/index.phtml';
        $_SERVER['SCRIPT_URL']      = '/path/to/test/index.phtml';
        $_SERVER['SCRIPT_URI']      = 'http://test.example.com/path/to/test/index.phtml';
        $_SERVER['HTTP_HOST']       = 'test.example.com';
        $_SERVER['HTTP_X_HOST']     = 'test.example.com';

        // client ip
        $_SERVER['HTTP_X_REAL_IP']  = '10.1.1.100';
        // simulate nginx proxy on same host
        $_SERVER['REMOTE_ADDR']     = '10.1.1.1';
        $_SERVER['SERVER_ADDR']     = '10.1.1.1';
        $_SERVER['HTTP_REFERER']    = 'http://example.com/test/referer';
    }

}

?>
