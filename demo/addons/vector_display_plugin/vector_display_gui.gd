extends Control

@onready var log_text: TextEdit = %LogText
@onready var frame_times_label: Label = %FrameTimesLabel
@onready var processing_headroom_label: Label = %ProcessingHeadroomLabel
@onready var asio_period_label: Label = %AsioPeriodLabel
@onready var asio_buffer_copy_label: Label = %AsioBufferCopyLabel
@onready var processing_time_label: Label = %ProcessingTimeLabel
@onready var samples_stats_label: Label = %SamplesStatsLabel
@onready var max_refresh_rate_h_slider: HSlider = %MaxRefreshRateHSlider
@onready var samples_header_label: Label = %SamplesHeaderLabel
@onready var refresh_header_label: Label = %RefreshHeaderLabel

const FRAME_TIMES_AMOUNT = 100

var _pressing_visible_chord: bool = false

var _total_avg_count: int
var _working_count: int

var _total_min: float
var _total_max: float
var _total_avg: float
var _working_min: float
var _working_max: float
var _working_cumulation: float

var _total_process_time_min: float
var _total_process_time_max: float
var _total_process_time_avg: float
var _working_process_time_min: float
var _working_process_time_max: float
var _working_process_time_cumulation: float

var _total_min_samples_headroom: int
var _total_max_blanking: int

var _timer: Timer = null


func _ready() -> void:
	max_refresh_rate_h_slider.value = VDFrameOutput.get_max_refresh_rate()
	clear_logging(false)


func _notification(what: int) -> void:
	match what:
		NOTIFICATION_VISIBILITY_CHANGED:
			if visible:
				$"../..".move_child.call_deferred($"..", -1)


func set_max_refresh_rate(value: float) -> void:
	VDFrameOutput.set_max_refresh_rate(value)


func start_asio() -> void:
	var vd: VectorDisplay = GlobalVectorDisplay
	vd.start_asio_output()

	if _timer == null:
		_timer = Timer.new()
		_timer.timeout.connect(clear_logging_timer_timeout)
		_timer.one_shot = true
		add_child(_timer)
		_timer.start(1)


func clear_logging_timer_timeout() -> void:
	clear_logging(false)
	if _timer:
		_timer.queue_free()
		_timer = null


func clear_logging(only_working: bool) -> void:
	_working_count = 0

	_working_min = 99999
	_working_max = 0
	_working_cumulation = 0

	_working_process_time_max = 0
	_working_process_time_min = 99999
	_working_process_time_cumulation = 0

	if !only_working:
		var vd: VectorDisplay = GlobalVectorDisplay
		vd.debug_reset_asio_profiling()
		_total_avg_count = 0

		_total_min = 99999
		_total_max = 0
		_total_avg = 0

		_total_process_time_min = 99999
		_total_process_time_max = 0
		_total_process_time_avg = 0

		_total_max_blanking = 0
		_total_min_samples_headroom = 99999999

		log_text.clear()
		frame_times_label.text = "\n\n\n\n\n\n"
		processing_headroom_label.text = "\n\n\n\n\n\n"
		processing_time_label.text = "\n\n\n\n\n\n"


func _process(_delta: float) -> void:
	var vd: VectorDisplay = GlobalVectorDisplay

	var input: bool = Input.is_key_pressed(KEY_G) && Input.is_key_pressed(KEY_H) && Input.is_key_pressed(KEY_K)
	if input && !_pressing_visible_chord:
		_pressing_visible_chord = true
		visible = !visible
	elif !input && _pressing_visible_chord:
		_pressing_visible_chord = false

	if vd.is_output_running():
		var starved_samples: int = vd.debug_get_last_starved_samples()
		if starved_samples != 0:
			log_text.insert_line_at(0, "%s: DAC starved of %s samples!!!" % [Time.get_time_string_from_system(), starved_samples])

		if vd.debug_get_blanking_sample_count() > _total_max_blanking:
			_total_max_blanking = vd.debug_get_blanking_sample_count()
		if vd.debug_get_wasted_sample_count() < _total_min_samples_headroom:
			_total_min_samples_headroom = vd.debug_get_wasted_sample_count()

		if visible:
			asio_period_label.text = "Total Min: %.3f ms\nTotal Max: %.3f ms" % [vd.debug_get_asio_min_time_between_buffer_switch(), vd.debug_get_asio_max_time_between_buffer_switch()]
			asio_buffer_copy_label.text = "Total Min: %.3f ms\nTotal Max: %.3f ms" % [vd.debug_get_asio_min_time_to_copy_buffers(), vd.debug_get_asio_max_time_to_copy_buffers()]
			processing_headroom_label.text = "*Total Min: %.2f ms\nTotal Max: %.2f ms" % [vd.debug_get_asio_min_headroom(), vd.debug_get_asio_max_headroom()]
			samples_header_label.text = "SAMPLES / %d:" % VDFrameOutput.get_target_buffer_size()
			samples_stats_label.text = "Headroom: %d | min: %d\nBlanking: %d | max: %d\n" % [vd.debug_get_wasted_sample_count(), _total_min_samples_headroom, vd.debug_get_blanking_sample_count(), _total_max_blanking]
			refresh_header_label.text = "REFRESH TIME / %.2f ms (%.1f Hz)" % [1000.0 / VDFrameOutput.get_max_refresh_rate(), VDFrameOutput.get_max_refresh_rate()]

		var frame_time: float = vd.get_previous_refresh_time()
		if frame_time < _working_min:
			_working_min = frame_time
		if frame_time > _working_max:
			_working_max = frame_time
		_working_cumulation += frame_time

		var processing_time: float = vd.debug_get_process_time()
		if processing_time < _working_process_time_min:
			_working_process_time_min = processing_time
		if processing_time > _working_process_time_max:
			_working_process_time_max = processing_time
		_working_process_time_cumulation += processing_time

		_working_count += 1

		if (_working_count == FRAME_TIMES_AMOUNT):
			var working_avg: float = _working_cumulation / FRAME_TIMES_AMOUNT
			if _working_min < _total_min:
				_total_min = _working_min
			if _working_max > _total_max:
				_total_max = _working_max

			var working_process_time_avg: float = _working_process_time_cumulation / FRAME_TIMES_AMOUNT
			if _working_process_time_min < _total_process_time_min:
				_total_process_time_min = _working_process_time_min
			if _working_process_time_max > _total_process_time_max:
				_total_process_time_max = _working_process_time_max

			_total_avg_count += 1
			_total_avg = lerpf(_total_avg, working_avg, 1.0 / _total_avg_count)
			_total_process_time_avg = lerpf(_total_process_time_avg, working_process_time_avg, 1.0 / _total_avg_count)

			if visible:
				frame_times_label.text = "Min: %.2f ms (%.1f Hz)\nAvg: %.2f ms (%.1f Hz)\nMax: %.2f ms (%.1f Hz)\nTotal Min: %.2f ms (%.1f Hz)\nTotal Avg: %.2f ms (%.1f Hz)\n*Total Max: %.2f ms (%.1f Hz)" % [_working_min * 1000.0, 1.0 / _working_min, working_avg * 1000.0, 1.0 / working_avg, _working_max * 1000.0, 1.0 / _working_max, _total_min * 1000.0, 1.0 / _total_min, _total_avg * 1000.0, 1.0 / _total_avg, _total_max * 1000.0, 1.0 / _total_max]
				processing_time_label.text = "Min: %.2f ms\nAvg: %.2f ms\nMax: %.2f ms\nTotal Min: %.2f ms\nTotal Avg: %.2f ms\n*Total Max: %.2f ms" % [_working_process_time_min, working_process_time_avg, _working_process_time_max, _total_process_time_min, _total_process_time_avg, _total_process_time_max]

			clear_logging(true)
