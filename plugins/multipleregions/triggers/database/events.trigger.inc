<?php

/*
* п╬я┌п╩п╬п╤п╣п╫п╫я▀п╣ п╪п╟п╫п╦п©я┐п╩я▐я├п╦п╦ я│ я─п╣пЁп╦п╬п╫п╟п╪п╦
* п©я─п╬я│я┌п╬ п╢п╬п╠п╟п╡п╩я▐п╣я┌ п╡ sys_regions_events п╥п╟п©п╦я│я▄
* п╡я│п╣  п╢п╣п╧я│я┌п╡п╦я▐ п╬я│я┐я┴п╣я│я┌п╡п╩я▐я▌я┌я│я▐ п╡ cronruns
*/

class PXTriggerDatabaseEvents extends PXAbstractDatabaseTrigger
{
    public const TYPE = PXMultiRegions::REGION_MARK;

    public function onCloneObject($db, $format, &$object, $donor)
    {
        if ($format->id != self::TYPE) return;

        if (is_callable(['PXEventsQueue', 'enqueue'])) {
            PXEventsQueue::enqueue([
                'callback' => ['class' => 'PXSREventsCallback', 'method' => 'addToObjects'],
                'params' => ['from' => $donor['id'], 'to' => $object['id']]]);
        }
    }
}

?>
