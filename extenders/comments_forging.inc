<?php
/**
 * Comments listing for single post for mobile platform.
 * It updates $post->extra_content_blocks array if comments are found.
 * Note: this is included directly, so no globals are imported.
 *
 * @package    BardCanvas
 * @subpackage mobile_comments
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config   $config
 * @var settings $settings
 * @var module   $this_module    self (mobile_comments)
 * @var module   $current_module mobile_posts
 * 
 * Imported globals:
 * @var post_record $post
 * @var feed_item   $item
 */

use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\comments\comment_record;
use hng2_modules\mobile_comments\comments_repository_extender;
use hng2_modules\mobile_controller\action_trigger;
use hng2_modules\mobile_controller\feed_item;
use hng2_modules\mobile_controller\content_block;
use hng2_modules\mobile_controller\feed_item_comment;
use hng2_modules\posts\post_record;

global $post, $config, $settings, $item, $modules, $account, $language;

$repository      = new comments_repository_extender();
$comments_module = $modules["comments"];

$allow_new_comments = $post->allow_comments == 1;
if( $post->allow_comments == 1 && $post->status != "published" )
{
    $allow_new_comments = false;
}
elseif(
    $settings->get("modules:comments.avoid_anonymous") == "true" &&
    $account->level < config::NEWCOMER_USER_LEVEL
) {
    $allow_new_comments = false;
}
else
{
    $days = $settings->get("modules:comments.disable_new_after");
    if( $post->allow_comments == 1 && ! empty($days) && ! empty($post->publishing_date) && $post->publishing_date != "0000-00-00 00:00:00" )
    {
        $boundary = date("Y-m-d H:i:s", strtotime("$post->publishing_date + $days days"));
        if( date("Y-m-d H:i:s") > $boundary ) $allow_new_comments = false;
    }
}
$item->allow_new_comments = $allow_new_comments;
if( $allow_new_comments ) $item->add_comment_action = new action_trigger(array(
    "action_id" => "comments:add",
    "params"    => array(
        "id_post"        => $post->id_post,
    ),
));

if( empty($config->globals["@mobile_comments:all_for_current_postsset"]) ) return;
if( empty($config->globals["@mobile_comments:all_for_current_postsset"][$post->id_post]) ) return;

/** @var comment_record[] $comments */
$comments = $config->globals["@mobile_comments:all_for_current_postsset"][$post->id_post];

/** @var comment_record[] $parents */
$parents = array();
foreach($comments as $comment) $parents[$comment->id_comment] = $comment;

$max_level = $settings->get("modules:comments.max_indent_level");
if( empty($max_level) ) $max_level = 5;

$limit_for_index = $settings->get("modules:comments.items_per_index_entry");
$item->comments_limit_for_index = empty($limit_for_index) ? 10 : $limit_for_index;

$comments = $repository->build_as_flattened_tree($comments);
foreach($comments as $comment)
{
    $instance = new feed_item_comment();
    $author   = $comment->get_author();
    
    $instance->id                = $comment->id_comment;
    $instance->creation_date     = $comment->creation_date;
    $instance->content           = externalize_urls($comment->get_processed_content());
    $instance->indent_level      = $comment->indent_level <= $max_level ? $comment->indent_level : $max_level;
    $instance->creation_ip       = $comment->creation_ip;
    $instance->creation_location = $comment->creation_location;
    $instance->indent_level      = $comment->indent_level <= $max_level ? $comment->indent_level : $max_level;
    
    $excerpt_length = $settings->get("modules:comments.excerpt_lengt_for_indexes");
    if( empty($excerpt_length) ) $excerpt_length = 100;
    $instance->excerpt = make_excerpt_of($comment->content, $excerpt_length);
    if( empty($instance->excerpt) ) $instance->excerpt = "...";
    else                            $instance->excerpt = convert_emojis($instance->excerpt);
    
    $avatar_url  = $author->get_avatar_url(true);
    $author_name = $author->_exists ? $author->get_processed_display_name() : $comment->author_display_name;
    if( ! empty($comment->parent_comment) )
    {
        $parent = $parents[$comment->parent_comment];
        if( ! empty($parent) )
        {
            if( empty($parent->id_author) )
            {
                $author_name .= "<span class='fa fa-chevron-right fa-fw'></span>{$parent->author_display_name}";
            }
            else
            {
                $parent_author = $parent->get_author();
                $author_name .= "<span class='fa fa-chevron-right fa-fw'></span>{$parent_author->get_processed_display_name()}";
            }
        }
    }
    $instance->author_id            = $author->_exists ? $author->id_account : 0;
    $instance->author_user_name     = $author->_exists ? $author->user_name : "";
    $instance->author_display_name  = $author->_exists ? $author->get_processed_display_name()
                                    : convert_emojis($comment->author_display_name);
    $instance->comment_reply_path   = $author_name;
    $instance->author_avatar        = $avatar_url;
    $instance->author_creation_date = $author->_exists ? $author->creation_date : null;
    $instance->author_level         = $author->_exists ? $author->level : 0;
    
    #
    # Extra details
    #
    
    $extra_details = array();
    if( $account->level >= $config::MODERATOR_USER_LEVEL )
    {
        if( ! empty($comment->id_author) )
            $extra_details[] = "
                <div class='fa-bulleted'>
                    <i class='fa fa-user fa-fw'></i>
                    {$modules["accounts"]->language->user_profile_page->info->level}
                    {$author->level} ({$author->get_role()})
                </div>
                <div class='fa-bulleted'>
                    <i class='fa fa-clock-o fa-fw'></i>
                    {$modules["accounts"]->language->user_profile_page->info->member_since}
                    <span class='convert-to-full-date'>{$author->creation_date}</span>
                </div>
            ";
        
        $extra_details[] = "
            <div class='fa-bulleted'>
                <i class='fa fa-globe fa-fw'></i>
                {$comment->creation_ip}
            </div>
        ";
        
        if( ! empty($comment->creation_location) )
            $extra_details[] = "
                <div class='fa-bulleted'>
                    <i class='fa fa-map-marker fa-fw'></i>
                    {$comment->creation_location}
                </div>
            ";
    }
    if( ! empty($extra_details) )
    {
        $instance->extra_content_blocks[] = new content_block(array(
            "title"    => "",
            "class"    => "framed_content extra_details color-gray",
            "contents" => implode("\n", $extra_details),
        ));
    }
    
    #
    # Actions
    #
    
    if( $post->allow_comments && $allow_new_comments && $account->id_account != $author->id_account )
    {
        # Reply
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "comments:reply",
            "caption"   => trim($comments_module->language->record_nav->actions->reply),
            "icon"      => "fa-reply",
            "class"     => "color-gray",
            "params"    => array(
                "id_post"        => $comment->id_post,
                "parent_comment" => $comment->id_comment,
            ),
        ));
    }
    
    if( $modules["contact"]->enabled && $account->id_account != $comment->id_author && $account->level < $config::MODERATOR_USER_LEVEL )
    {
        # Report
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "comments:report",
            "caption"   => trim($this_module->language->report),
            "icon"      => "fa-exclamation-circle",
            "class"     => "color-pink",
            "params"    => array(
                "id" => $comment->id_comment
            ),
        ));
    }
    
    if( $author->can_interact_in_pms() )
    {
        # PM
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "messaging:compose",
            "caption"   => trim($language->contact->pm->caption),
            "icon"      => "fa-inbox",
            "class"     => "color-gray",
            "params"    => array(
                "target"      => $author->id_account,
                "target_name" => $author->display_name,
            ),
        ));
    }
    
    if( is_comment_editable($comment) )
    {
        # Edit
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "comments:edit",
            "caption"   => trim($comments_module->language->record_nav->actions->edit),
            "icon"      => "fa-pencil-square-o",
            "class"     => "color-gray",
            "params"    => array(
                "id_comment" => $comment->id_comment
            ),
        ));
    }
    
    if( $comment->can_be_deleted() )
    {
        # Trash
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "comments:trash",
            "caption"   => trim($comments_module->language->record_nav->actions->trash),
            "icon"      => "fa-trash",
            "class"     => "color-red",
            "params"    => array(
                "id_comment" => $comment->id_comment,
            ),
        ));
    }
    
    #
    # Admin actions
    #
    
    if(
        $account->level >= config::COADMIN_USER_LEVEL &&
        $account->id_account != $author->id_account &&
        $author->level < $config::MODERATOR_USER_LEVEL
    ) {
        # Reject comment
        $instance->action_triggers[] = new action_trigger(array(
            "action_id" => "comments:reject",
            "caption"   => trim($comments_module->language->record_nav->actions->reject),
            "icon"      => "fa-times",
            "class"     => "color-orange",
            "params"    => array(
                "id_comment" => $comment->id_comment,
            ),
        ));
        
        # Disable author account
        if( $author->_exists )
        {
            $instance->action_triggers[] = new action_trigger(array(
                "action_id" => "accounts:disable",
                "caption"   => trim($this_module->language->disable_author),
                "icon"      => "fa-user-times",
                "class"     => "color-red",
                "options"   => array(
                    "remove_parent_on_success" => true,
                ),
                "params"    => array(
                    "id_account" => $author->id_account,
                ),
            ));
        }
    }
    
    $instance->has_actions = count($instance->action_triggers) > 0;
    
    #
    # Add it to the collection
    #
    
    $item->comments[] = $instance;
}
