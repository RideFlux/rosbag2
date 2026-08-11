# Copyright 2026 RideFlux Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from argparse import ArgumentParser
from pathlib import Path
import tempfile
from unittest.mock import patch

from ros2bag.verb.record import RecordVerb


def test_overwrite_removes_existing_output_before_recording():
    with tempfile.TemporaryDirectory() as temporary_directory:
        output = Path(temporary_directory) / 'bag'
        output.mkdir()
        (output / 'old-data').write_text('old')

        verb = RecordVerb()
        parser = ArgumentParser()
        verb.add_arguments(parser, 'ros2 bag record')
        args = parser.parse_args(['--all', '--output', output.as_posix(), '--overwrite'])

        with patch('ros2bag.verb.record.Recorder') as recorder:
            result = verb.main(args=args)

        assert result is None
        assert not output.exists()
        recorder.return_value.record.assert_called_once()
