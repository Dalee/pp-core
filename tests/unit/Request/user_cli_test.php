<?php
require_once BASEPATH . '/libpp/lib/Request/classes.inc';

class PXRequestUserCLITest extends UnitTestCase {

    private static $original_bucks_server;

    function setUp() {
        self::$original_bucks_server = $_SERVER;
        self::setUp_cli_env();
    }
    
    /* restore original environment after each test */
    function tearDown() {
        $_SERVER = self::$original_bucks_server;
    }

    function test_Instance_Should_be_PXRequestUser() {
        $request = new PXRequestUser();
        $this->assertIsA($request, 'PXRequestUser');
    }

    
    function test_default_HTTPMethod_should_be_CLI() {
        $this->assertEqual(PXRequestUser::GetHttpMethod(), 'CLI');
    }
    
    function test_default_HTTPReferer_should_be_NULL() {
        $this->assertNull(PXRequestUser::getHttpReferer());
    }
    
    //TODO: why should we want two referer functions?
    function test_default_Referer_should_be_NULL() {
        $request = new PXRequestUser();
        $this->assertNull($request->getReferer());
    }

    function test_default_HTTPRequestURI_should_be_NULL() {
        $this->assertNull(PXRequestUser::getRequestUri());
    }

    function test_GET_HTTPMethod_should_be_GET() {
        $_SERVER['REQUEST_METHOD'] = 'GET';
        $this->assertEqual(PXRequestUser::getHttpMethod(), 'GET');
    }
    
    function test_POST_HTTPMethod_should_be_POST() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $this->assertEqual(PXRequestUser::getHttpMethod(), 'POST');
    }
    
    private static function setUp_cli_env() {
        $_SERVER['REQUEST_METHOD'] = NULL;
        $_SERVER['HTTP_REFERER']   = NULL;
        $_SERVER['REQUEST_URI']    = NULL;
        $_SERVER['SCRIPT_NAME']    = NULL;
        $_SERVER['HTTP_X_HOST']    = NULL;
        $_SERVER['HTTP_HOST']      = NULL;
    }

}

?>
