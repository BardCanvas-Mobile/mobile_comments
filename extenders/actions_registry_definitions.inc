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

global $manifest, $modules;

$comments_module = $modules["comments"];

# Reply
$manifest->actionsRegistry["comments:reply"] = new action(array(
    "id"          => "comments:reply",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/add.php",
    "call_method" => "post",
));

if( $modules["contact"]->enabled )
{
    # Report
    $manifest->actionsRegistry["comments:report"] = new action(array(
        "id"          => "comments:report",
        "module_name" => "contact",
        "script_url"  => "{$modules["contact"]->get_url(true)}/index_bcm.php?action=report&type=comment&stop_on_success=true",
        "call_method" => "frame",
    ));
}

# Edit
$manifest->actionsRegistry["comments:edit"] = new action(array(
    "id"          => "comments:edit",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/save.php",
    "call_method" => "post",
));

# Trash
$manifest->actionsRegistry["comments:trash"] = new action(array(
    "id"          => "comments:trash",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/toolbox.php?action=change_status&new_status=trashed",
    "options"     => array(
        "remove_parent_on_success" => true,
        "requires_confirmation"    => true,
        "confirmation_message"     => unindent($this_module->language->messages->prompts->trash),
    ),
));

# Reject
$manifest->actionsRegistry["comments:reject"] = new action(array(
    "id"          => "comments:reject",
    "module_name" => $comments_module->name,
    "script_url"  => "{$comments_module->get_url(true)}/scripts/toolbox.php?action=change_status&new_status=rejected",
    "options"     => array(
        "remove_parent_on_success" => true,
        "requires_confirmation"    => true,
        "confirmation_message"     => unindent($this_module->language->messages->prompts->reject),
    ),
));

