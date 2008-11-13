<?php 
$taxonomy = array();
foreach($node->taxonomy as $k=>$v)
    $taxonomy[$v->vid] = $v;

if($page):?>

  <div class="node node-crime<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
        
	<?php if (!$teaser): ?>
	<?php if ($submitted): ?>
        <div class="metanode"><p><span class="date"><?php print format_date($node->created, 'custom', "d F Y");?></span></p></div> 
    <?php endif; ?>
    <?php endif; ?>
    
    <div class="content">
    
        <?php
        global $user;
        $user = user_load(array('uid'=>$user->uid));
                
        //$info = _dccrime_userinfo(array('uid'=>$user->uid,'mail'=>$user->mail));        
        
        $markers = array();
        
        $center_lat = $node->location['latitude'];
        $center_long = $node->location['longitude'];
        $markers[] = array(
			'latitude' => $node->location['latitude'],
			'longitude' => $node->location['longitude'],
			'label' => $taxonomy[1]->name,
			'opts' => array('title' => $taxonomy[1]->name),
		);

        /*
        // disable marking the user's house for now
        if(isset($user->location['latitude']))
        {
            $center_lat = ($center_lat + $user->location['latitude']) / 2.0;
            $center_long = ($center_long + $user->location['longitude']) / 2.0;
            $markers[] = array(
    			'latitude' => $user->location['latitude'],
    			'longitude' => $user->location['longitude'],
    			'label' => t('Your home'),
    			'opts' => t('Your home'),
    		);
        }
        */
        
        $map = array(
    			'#type' => 'gmap',
    			'#map' => 'project-gmap',
    			'#settings' => array(
    				'center' => ($center_lat . ',' . $center_long),					
    				'latitude' => $center_lat,
    				'longitude' => $center_long,
    				'zoom' => 14,
    				'align' => 'right',
    				'width' => '300px',
    				'height' => '250px',
    				'markers' => $markers,			
    			),
    		);
        print theme('gmap',$map);
        ?>
    
        <div class="crime-data-item offense"><?php print l($taxonomy[1]->name,drupal_get_path_alias('taxonomy/term/'.$taxonomy[1]->tid));?></div>
        
        <div class="crime-data-item address"><?php if(strlen($node->field_address[0]['view'])): print $node->field_address[0]['view']; endif;?></div>
        
        <div class="crime-data-item method"><label>Method:</label> <?php print l($taxonomy[2]->name,drupal_get_path_alias('taxonomy/term/'.$taxonomy[2]->tid));?></div>
        
        <div class="crime-data-item anc"><label>ANC:</label> <?php print l($taxonomy[3]->name,drupal_get_path_alias('taxonomy/term/'.$taxonomy[3]->tid));?></div>

        <div class="crime-data-item district"><label>Police District:</label> <?php print l($taxonomy[4]->name,drupal_get_path_alias('taxonomy/term/'.$taxonomy[4]->tid));?></div>
        
        <div class="crime-data-item"><?php print $node->content['body']['#value'];?></div>

    </div>
    
    <?php if (!$teaser): ?>
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
    <?php endif; ?>
    
    <?php if ($teaser): ?>
    <?php if ($links) { ?><div class="linksteaser"><div class="links"><?php print $links?></div></div><?php }; ?>
    <?php endif; ?>
    
    
  </div>

<?php else:?>

<div class="node node-crime<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
    <?php 
    $title = $taxonomy[1]->name . (strlen($node->field_address[0]['view']) ? (' - ' . $node->field_address[0]['view']) : '') . ' - ' . format_date($node->created);
    print l($title,drupal_get_path_alias('node/'.$node->nid));
    ?>
</div>

<?php endif;?>