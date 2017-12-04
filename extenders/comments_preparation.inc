<?php
/**
 * Comments preparation for posts - Preloads comments for all posts
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
 * @var post_record[] $posts
 */

use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\comments\comments_repository;
use hng2_modules\posts\post_record;

global $posts, $config, $settings, $modules, $account;

$post_ids = array();
foreach($posts as $post) $post_ids[] = $post->id_post;

$repository   = new comments_repository();
$all_comments = $repository->get_for_multiple_posts($post_ids);
$config->globals["@mobile_comments:all_for_current_postsset"] =& $all_comments;
