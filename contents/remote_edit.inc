<?php
/**
 * Comment editing form
 *
 * @package    BardCanvas
 * @subpackage mobile_comments
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Imported globals:
 * @var post_record $post
 * @var config      $config
 *
 * Trailing vars:
 * @var module         $this_module    self (comments)
 * @var module         $current_module posts|comments
 * @var comment_record $comment
 *                             
 * @var bool   $ajax_calling
 */

use hng2_base\config;
use hng2_base\module;
use hng2_modules\comments\comment_record;
use hng2_modules\posts\post_record;

global $post, $template, $language, $settings, $account;
?>

<h1><?= $current_module->language->editing_comment ?></h1>

<script type="text/javascript">
    function prepare_comment_edit_serialization($form)
    {
        $form.find('textarea[class*="tinymce"]').each(function()
        {
            var id      = $(this).attr('id');
            var editor  = tinymce.get(id);
            var content = editor.getContent();
            $(this).val( content );
        });
        
        $('#error_message').hide('scale', {direction: 'up'});
    }
    
    function prepare_comment_edit_submission(data, $form)
    {
        $.blockUI(blockUI_default_params);
    }
    
    function process_comment_edit_submission(response, status, xhr, $form)
    {
        if( response.indexOf('OK') < 0 )
        {
            $.unblockUI();
            $('#error_message').html(sprintf('<i class="fa fa-warning"></i> %s', response.replace(/\n/g, '<br>\n'))).show();
            
            return;
        }
        
        close_edit_form();
    }
    
    function close_edit_form()
    {
        parent.postMessage('BCM:CLOSE_FRAME', '*');
        location.href = "/BCM/CLOSE_FRAME";
    }
    
    $(document).ready(function()
    {
        $('#post_comment').ajaxForm({
            target:          '#post_comment_target',
            beforeSerialize: prepare_comment_edit_serialization,
            beforeSubmit:    prepare_comment_edit_submission,
            success:         process_comment_edit_submission
        });
    });
</script>

<form name="post_comment" id="post_comment" method="post"
      action="<?= $config->full_root_path ?>/comments/scripts/save.php">
    
    <input type="hidden" name="id_comment"        value="<?= $comment->id_comment ?>">
    <input type="hidden" name="bcm_platform"      value="<?= $_REQUEST["bcm_platform"] ?>">
    <input type="hidden" name="bcm_version"       value="<?= $_REQUEST["bcm_version"] ?>">
    <input type="hidden" name="bcm_access_token"  value="<?= $_REQUEST["bcm_access_token"] ?>">
    <input type="hidden" name="bcm_force_session" value="<?= $_REQUEST["bcm_force_session"] ?>">
    <input type="hidden" name="bcm_output_type"   value="<?= $_REQUEST["bcm_output_type"] ?>">
    
    <div id="error_message" class="framed_content state_ko" style="display: none;"></div>
    
    <div class="field tinymce_container">
        <textarea name="content" class="tinymce"><?= htmlspecialchars($comment->content) ?></textarea>
    </div>
    
    <p>
        <button type="submit">
            <span class="fa fa-play"></span>
            <?= $language->submit ?>
        </button>
        
        <button onclick="close_edit_form(); return false">
            <span class="fa fa-ban"></span>
            <?= $language->cancel ?>
        </button>
        
    </p>
</form>

<div id="post_comment_target" style="display: none;"></div>
