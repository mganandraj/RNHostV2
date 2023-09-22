export const nativeNamespace = 'reacthostapp';
import { int32 } from '@office-iss/reka';

export interface NativeService {
    tryCall(input1: string, input2: int32): void;
}