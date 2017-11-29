<?php
/**
 * Mobile Controller Action Definitions Extender
 * Registers actions for the mobile controller.
 *
 * @package    BardCanvas
 * @subpackage mobile_comments
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_modules\mobile_controller\action;

global $manifest, $this_module, $modules;

$comments_module = $modules["comments"];

$manifest->actionsRegistry["comments:reply"] = new action(array(
    "id"          => "comments:reply",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/add.php",
    "call_method" => "post",
));

$manifest->actionsRegistry["comments:quote"] = new action(array(
    "id"          => "comments:quote",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/add.php",
    "call_method" => "post",
));

# TODO: This is not defined anwhere!
$manifest->actionsRegistry["comments:report"] = new action(array(
    "id"          => "comments:report",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/add.php?send_as_report=true",
    "call_method" => "post",
));

$manifest->actionsRegistry["comments:edit"] = new action(array(
    "id"          => "comments:edit",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/save.php",
    "call_method" => "post",
));

$manifest->actionsRegistry["comments:trash"] = new action(array(
    "id"          => "comments:trash",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/toolbox.php?action=change_status&new_status=trashed",
));

$manifest->actionsRegistry["comments:reject"] = new action(array(
    "id"          => "comments:reject",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/toolbox.php?action=change_status&new_status=rejected",
));

