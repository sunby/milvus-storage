// Copyright 2023 Zilliz
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "arrow/table.h"
#include "common/result.h"
namespace milvus_storage {

class Reader {
  public:
  virtual void Close() = 0;

  virtual Result<std::shared_ptr<arrow::Table>> ReadByOffsets(std::vector<int64_t>& offsets) = 0;
};
}  // namespace milvus_storage