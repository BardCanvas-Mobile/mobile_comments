<?php
namespace hng2_modules\mobile_comments;

use hng2_modules\comments\comments_repository;

class comments_repository_extender extends comments_repository
{
    public function build_as_flattened_tree( $comments )
    {
        $comments = array_reverse($comments);
        $tree     = $this->build_tree($comments);
        $final    = $this->flatten_tree($tree);
        
        return $final;
    }
}
