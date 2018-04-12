#ifndef __sender_congestion_controller_h_
#define __sender_congestion_controller_h_

#include "feedback_adapter.h"
#include "delay_base_bwe.h"
#include "bitrate_controller.h"
#include "pace_sender.h"
#include "ack_bitrate_estimator.h"

typedef struct
{
	int							accepted_queue_ms;			/*视频报文在发送queue的最大延迟*/
	int							was_in_alr;
	delay_base_bwe_t*			bwe;						/*基于延迟的带宽评估器*/
	bitrate_controller_t*		bitrate_controller;			/*码率控制器，会根据bwe、ack rate和loss进行综合码率调节*/
	ack_bitrate_estimator_t*	ack;						/*远端确认收到的数据带宽评估器*/
	pace_sender_t*				pacer;						/*发送端的步长控制器*/
	feedback_adapter_t			adapter;					/*处理反馈信息的适配器*/

}sender_cc_t;

sender_cc_t*					sender_cc_create(void* trigger, bitrate_changed_func bitrate_cb, void* handler, pace_send_func send_cb, int queue_ms);
void							sender_cc_destroy(sender_cc_t* cc);

void							sender_cc_heartbeat(sender_cc_t* cc, int64_t cur_ts);

/*packet_id是报文序号，相当于RTP的头中的SEQ*/
void							sender_cc_add_pace_packet(sender_cc_t* cc, uint32_t packet_id, int retrans, size_t size, int64_t now_ts);
/*这里的seq是transport的自增长ID，即使包重发，这个ID也是不一样的*/
void							sender_on_send_packet(sender_cc_t* cc, uint16_t seq, size_t size);

void							sender_on_feedback(sender_cc_t* cc, bin_stream_t* strm);

void							sender_cc_update_rtt(sender_cc_t* cc, int32_t rtt);
void							sender_cc_set_bitrates(sender_cc_t* cc, uint32_t min_bitrate, uint32_t start_bitrate, uint32_t max_bitrate);

int64_t							sender_cc_get_pacer_queue_ms(sender_cc_t* cc);
int64_t							sender_cc_get_first_packet_ts(sender_cc_t* cc);


#endif 



