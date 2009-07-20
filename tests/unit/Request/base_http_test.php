<?php
require_once BASEPATH . '/libpp/lib/Request/classes.inc';

class PXRequestBaseTest extends UnitTestCase {
    
    private static $original_bucks_server;
    private static $original_bucks_get;
    private static $original_bucks_post;
    private static $original_bucks_cookie;
    private static $original_bucks_files;

    function setUp() {
        self::$original_bucks_server = $_SERVER;
        self::$original_bucks_get    = $_GET;
        self::$original_bucks_post   = $_POST;
        self::$original_bucks_cookie = $_COOKIE;
        self::$original_bucks_cookie = $_FILES;

        self::setUp_http_env();
    }
    
    /* restore original environment after each test */
    function tearDown() {
        $_SERVER = self::$original_bucks_server;
        $_GET    = self::$original_bucks_get; 
        $_POST   = self::$original_bucks_post; 
        $_COOKIE = self::$original_bucks_cookie; 
        $_FILES  = self::$original_bucks_files; 
    }

    function test_Instance_Should_be_PXRequestBase() {
        $request = new PXRequestBase();
        $this->assertIsA($request, 'PXRequestBase');
    }
    
    function test_default_HTTPMethod_should_be_GET() {
        $this->assertEqual(PXRequestBase::GetHttpMethod(), 'GET');
    }
    
    function test_default_HTTPReferer_should_be_defined() {
        $this->assertEqual(PXRequestBase::getHttpReferer(), 'http://example.com/test/referer');
    }
    
    //TODO: why should we want two referer functions?
    function test_default_Referer_should_be_set() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getReferer(), 'http://example.com/test/referer');
    }

    function test_default_HTTPRequestURI_should_be_defined() {
        $this->assertEqual(PXRequestBase::getRequestUri(), '/path/to/test/index.phtml?a=1&b=2');
    }

    function test_GET_HTTPMethod_should_be_GET() {
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $this->assertEqual(PXRequestBase::getHttpMethod(), 'GET');
    }
    
    function test_POST_HTTPMethod_should_be_POST() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $this->assertEqual(PXRequestBase::getHttpMethod(), 'POST');
    }

    /* 
     *
     * PXRequestBase->getPath()
     *
     */
    function test_GetPath_should_ignore_index_phtml() {
        $request = new PXRequestBase();
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

        $path_parts = array('path', 'to', 'test');
            
        foreach($known_indexes as $index) {
            $_SERVER['SCRIPT_NAME'] = '/path/to/test/' . $index;            
            $request = new PXRequestBase();
            $this->assertEqual($request->getPath(), $path_parts);
        }
    }

    function test_GetPath_should_not_ignore_other_extensions() {
        $_SERVER['SCRIPT_NAME'] = '/path/to/test/readme.doc';
        $request = new PXRequestBase();
        $path_parts = array('path', 'to', 'test', 'readme.doc');
        $this->assertEqual($request->getPath(), $path_parts);
    }


    /*
     *
     * PXRequestBase::getHttpHost() 
     *
     */
    function test_default_getHttpHost_sould_be_defined() {
        $this->assertEqual(PXRequestBase::getHttpHost(), 'test.example.com');
    }

    function test_getHttpHost_sould_be_overrided() {
        $_SERVER['HTTP_X_HOST'] = 'overrided.example.com';
        $this->assertEqual(PXRequestBase::getHttpHost(), 'overrided.example.com');
    }

    function test_getHttpHost_sould_handle_multiple_hosts() {
        $_SERVER['HTTP_X_HOST'] = 'maybe.example.com, correct.example.com';
        $this->assertEqual(PXRequestBase::getHttpHost(), 'correct.example.com');
    }
    
    /*
     *
     * PXRequestBase::getRemoteAddr()
     *
     */
    function test_default_getRemoteAddr_sould_be_defined() {
        $this->assertEqual(PXRequestBase::getRemoteAddr(), '10.1.1.100');
    }
    
    function test_getRemoteAddr_sould_be_overrided() {
        $_SERVER['HTTP_X_REAL_IP'] = '192.168.1.1';
        $this->assertEqual(PXRequestBase::getRemoteAddr(), '192.168.1.1');
    }
    
    function test_getRemoteAddr_sould_handle_multiple_addreses() {
        $_SERVER['HTTP_X_REAL_IP'] = '10.10.10.10, 20.20.20.20, 192.168.1.1';
        $this->assertEqual(PXRequestBase::getRemoteAddr(), '192.168.1.1');
    }
    
    /*
     *
     * PXRequestBase GET variables
     *
     */
    function test_getGetVar_should_return_value() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getGetVar('a'), 1);
        $this->assertEqual($request->getGetVar('b'), 2);
    }
    
    function test_getAllGetData_should_return_values() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getAllGetData(), array('a' => 1, 'b' => 2));
    }
    
    function test_isSetVar_with_get_variables() {
        $request = new PXRequestBase();
        $this->assertTrue($request->isSetVar('a'));
        $this->assertFalse($request->isSetVar('not-defined-variable'));
    }
    
    function test_setGetVar_should_success() {
        $request = new PXRequestBase();
        $request->setVar('get-variable', 'BANG! BANG!');
        $this->assertEqual($request->getGetVar('get-variable'), 'BANG! BANG!');
        $this->assertNull($request->getPostVar('get-variable'));
    }
    

    /*
     *
     * PXRequestBase POST variables
     *
     */
    function test_getPostVar_should_return_value() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getPostVar('x'), 10);
        $this->assertEqual($request->getPostVar('y'), 20);
    }
    
    function test_getAllPostData_should_return_values() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getAllPostData(), array('x' => 10, 'y' => 20));
    }
    function test_isSetVar_with_post_variables() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $request = new PXRequestBase();
        $this->assertTrue($request->isSetVar('x'));
        $this->assertFalse($request->isSetVar('not-defined-variable'));
    }

    function test_setPostVar_should_success() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $request = new PXRequestBase();
        $request->setVar('post-variable', 'TADAM');
        $this->assertEqual($request->getPostVar('post-variable'), 'TADAM');
        $this->assertNull($request->getGetVar('post-variable'));
    }
    

    /*
     *
     * PXRequestBase COOKIE variables
     *
     */
    function test_getCookieVar_should_return_value() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getCookieVar('uid'), '1234567890');
        $this->assertEqual($request->getCookieVar('guid'), 'ABCDEFABCDEF');
    }
    
    function test_getAllCookieData_should_return_values() {
        $request = new PXRequestBase();
        $this->assertEqual($request->getAllCookieData(), array('uid' => 1234567890, 'guid' => 'ABCDEFABCDEF'));
    }

    /* 
        TODO this code should work some day

    function test_setCookieVar_should_success() {
        $request = new PXRequestBase();
        $request->setCookieVar('updated', 'ONE-TWO-THREE');
        $this->assertEqual($request->getCookieVar('updated'), 'ONE-TWO-THREE');
        $this->assertNotNull($request->getCookieVar('charcheck'));
    }
    
     */

    /*
     *
     * PXRequestBase FILES variables
     *
     */

    function test_getUploadFile_should_return_value() {
        $request = new PXRequestBase();
        $expected_array = array('name' => 'avatar.png');
        $this->assertEqual($request->getUploadFile('avatar'), $expected_array);
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

        $_GET['a'] = 1;
        $_GET['b'] = 2;
        
        $_POST['x'] = 10;
        $_POST['y'] = 20;
        
        $_COOKIE['uid'] =  '1234567890';
        $_COOKIE['guid'] = 'ABCDEFABCDEF';

        $_FILES['avatar'] = array();
        $_FILES['avatar']['name'] = 'avatar.png';
    
    }

}

?>
