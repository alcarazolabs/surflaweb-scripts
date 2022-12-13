import boto3
import time
import urllib
import json
    
AWS_DEFAULT_REGION = "us-east-1"
AWS_ACCESS_KEY_ID ="XXX"
AWS_SECRET_ACCESS_KEY = "XXXX"

#transcribe_client = boto3.client('transcribe')
# connect by using credentias
transcribe_client = boto3.client('transcribe', aws_access_key_id=AWS_ACCESS_KEY_ID,
                                                    aws_secret_access_key=AWS_SECRET_ACCESS_KEY,
                                                    region_name=AWS_DEFAULT_REGION)

JOB_NAME = "Example-job"

# Obtener el nombre/path del ultimo archivo subido al bucket.
s3 = boto3.client('s3')
objs = s3.list_objects_v2(Bucket='max9814audiofiles')['Contents']
last_added = [obj['Key'] for obj in sorted(objs, key=get_last_modified)][-1]

print(last_added)


def transcribe_file(job_name, file_uri, transcribe_client):
    transcribe_client.start_transcription_job(
        TranscriptionJobName=job_name,
        Media={'MediaFileUri': file_uri},
        MediaFormat='wav',
        LanguageCode='es-ES' #english: en-US
    )

    max_tries = 60
    while max_tries > 0:
        max_tries -= 1
        job = transcribe_client.get_transcription_job(TranscriptionJobName=job_name)
        job_status = job['TranscriptionJob']['TranscriptionJobStatus']
        if job_status in ['COMPLETED', 'FAILED']:
            print(f"Job {job_name} is {job_status}.")
            if job_status == 'COMPLETED':
                response = urllib.request.urlopen(job['TranscriptionJob']['Transcript']['TranscriptFileUri'])
                data = json.loads(response.read())
                text = data['results']['transcripts'][0]['transcript']
                print("========== below is output of speech-to-text ========================")
                print(text)
                print("=====================================================================")
            break
        else:
            print(f"Waiting for {job_name}. Current status is {job_status}.")
        time.sleep(10)


def main():
    file_uri = 's3://max9814audiofiles/test.wav'
    # Delete the job is already exists to prevent duplicate jobs problem error:
    transcribe_client.delete_transcription_job(TranscriptionJobName=JOB_NAME)
    transcribe_file(JOB_NAME, file_uri, transcribe_client)


if __name__ == '__main__':
    main()
