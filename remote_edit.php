<?php
/**
 * Remote comment editor
 *
 * @package    BardCanvas
 * @subpackage mobile_comments
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * @var account  $account
 * 
 * $_GET|$_POST params
 * @param int "id_comment"
 */

use hng2_base\account;
use hng2_base\template;
use hng2_modules\comments\comments_repository;
use hng2_modules\mobile_controller\toolbox;

include "../config.php";
include "../includes/bootstrap.inc";
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');

$toolbox = new toolbox();
$toolbox->output_type = "HTML";
$toolbox->open_session();

$_REQUEST["id_comment"] = $_REQUEST["id_comment"] + 0;

if( empty($_REQUEST["id_comment"]) )
    die( trim($current_module->language->messages->comment_id_not_provided) );

$repository = new comments_repository();
$comment = $repository->get($_REQUEST["id_comment"]);
if( is_null($comment) )
    die( trim($current_module->language->messages->comment_not_found) );

$template->page_contents_include = "remote_edit.inc";
$template->set_page_title($current_module->language->editing_comment);
include "{$template->abspath}/popup.php";
