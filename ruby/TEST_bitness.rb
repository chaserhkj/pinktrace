#!/usr/bin/env ruby
# coding: utf-8
# vim: set sw=2 sts=2 et nowrap fenc=utf-8 :

require 'test/unit'

$:.insert(0, '.libs')
require 'PinkTrace'

class TestPinkBitness < Test::Unit::TestCase
  def test_bitness_get_invalid
    assert_raise ArgumentError do
      PinkTrace::Bitness.get
    end
    assert_raise ArgumentError do
      PinkTrace::Bitness.get 0, 1
    end
    assert_raise TypeError do
      PinkTrace::Bitness.get 'pink'
    end
  end

  def test_bitness_get
    pid = fork do
      PinkTrace::Trace.me
      Process.kill 'STOP', Process.pid
    end
    Process.waitpid pid
    bitness = PinkTrace::Bitness.get pid
    assert(bitness == PinkTrace::Bitness::DEFAULT, "Wrong bitness, expected: #{PinkTrace::Bitness::DEFAULT} got: #{bitness}")

    begin PinkTrace::Trace.kill pid
    rescue Errno::ESRCH ;end
  end
end
